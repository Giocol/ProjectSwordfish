#include "ProceduralLimbManager.h"

#include "Components/PoseableMeshComponent.h"
#include "Entomon/Spider/MultiLeggedPawn.h"

UProceduralLimbManager::UProceduralLimbManager() {
	//AutoDetectLimbs();
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UProceduralLimbManager::BeginPlay() {
	Super::BeginPlay();

	if(auto o = Cast<AMultiLeggedPawn>(GetOwner()))
		AutoDetectLimbs(o->GetMesh());
	//FindLegs();
}

void UProceduralLimbManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	for(int i = 0; i < Legs.Num(); ++i) {
		Legs[i].UpdateIK(Mesh, true);
		FVector Displacement;
		if(Legs[i].PrefersTargetRelocation(Mesh, 15.f, Displacement)) {
			FVector ComponentSpaceLocation = Legs[i].RestingTargetLocation - 1 * Displacement;
			FVector WorldSpaceLocation = Mesh->GetComponentTransform().TransformPosition(ComponentSpaceLocation);
			//Legs[i].IKTarget->SetWorldLocation(WorldSpaceLocation);
			GEngine->AddOnScreenDebugMessage(i, .1f, FColor::Emerald, TEXT("I want to relocate!"));
		}
	}
}

void UProceduralLimbManager::AutoDetectLimbs(UPoseableMeshComponent* InMesh) {
	if(!InMesh) return;
	Mesh = InMesh;
	Legs.Empty();

	TArray<FName> Bones;
	Mesh->GetBoneNames(Bones);
	//Mesh->GetSkinnedAsset()->GetSkeleton()->

	for(FName Bone : Bones) {
		auto boneAsString = Bone.ToString();
		auto templateName = EndEffectorsName.ToString();
		FString endName = "_end";
		auto foundTip = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		auto foundEnd = boneAsString.Find(*endName, ESearchCase::CaseSensitive);

		if(foundTip != INDEX_NONE && foundEnd != INDEX_NONE) {
			Legs.Add(FLeg());
			Legs.Last().RestPose.Add(FLimbSegment(Bone, FRotator::ZeroRotator));
			Legs.Last().IKTarget = Cast<USceneComponent>(
				GetOwner()->AddComponentByClass(USceneComponent::StaticClass(),
					true,
					FTransform::Identity,
					false));
			Legs.Last().IKTarget->SetWorldLocation(Mesh->GetBoneLocationByName(Bone, EBoneSpaces::WorldSpace));
			Legs.Last().RestingTargetLocation = Mesh->GetBoneLocationByName(Bone, EBoneSpaces::ComponentSpace);
			
			auto name = FString::Printf(TEXT("IK_"));
			boneAsString.RemoveAt(foundTip, templateName.Len());
			boneAsString.RemoveFromEnd("_end");
			name.AppendChars(*boneAsString, boneAsString.Len());
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
			//Legs.Last().IKTarget->AttachToComponent(GetOwner()->GetRootComponent(), AttachmentRules);
			Legs.Last().IKTarget->Rename(*name);
			GetOwner()->AddInstanceComponent(Legs.Last().IKTarget);
			//Legs.Last().IKTarget = Mesh->GetBoneLocationByName(Bone, EBoneSpaces::WorldSpace);
			RecurseToHip(Bone);
		}
	}
	for(auto& Leg : Legs) {
		for(int i = 1; i < Leg.RestPose.Num(); ++i) {
			FVector Displacement = Mesh->GetBoneLocationByName(Leg.RestPose[i-1].Name, EBoneSpaces::ComponentSpace);
				- Mesh->GetBoneLocationByName(Leg.RestPose[i].Name, EBoneSpaces::ComponentSpace);
			Leg.RestPose[i].Length = Displacement.Length();
		}
		Leg.Bones = Leg.RestPose;
	}
}

void UProceduralLimbManager::RecurseToHip(FName From) {
	
	FName Parent = Mesh->GetParentBone(From);
	if(Parent.IsNone())
		return;
	Legs.Last().RestPose.Add(
		FLimbSegment(Parent, Mesh->GetBoneRotationByName(Parent, EBoneSpaces::ComponentSpace)));
	auto boneAsString = Parent.ToString();
	auto templateName = HipJointsName.ToString();
	auto s = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
	if(s == INDEX_NONE)
		RecurseToHip(Parent);
}
