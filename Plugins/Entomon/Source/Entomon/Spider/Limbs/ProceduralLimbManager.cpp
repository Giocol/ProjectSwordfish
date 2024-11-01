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
	for(int i = 0; i < Limbs.Num(); ++i) {
		Limbs[i]->UpdateIK(Mesh, true);
		FVector Displacement;
		if(Limbs[i]->PrefersTargetRelocation(Mesh, 15.f, Displacement)) {
			FVector ComponentSpaceLocation = Limbs[i]->RestingTargetLocation - 1 * Displacement;
			FVector WorldSpaceLocation = Mesh->GetComponentTransform().TransformPosition(ComponentSpaceLocation);
			//Legs[i].IKTarget->SetWorldLocation(WorldSpaceLocation);
			GEngine->AddOnScreenDebugMessage(i, .1f, FColor::Emerald, TEXT("I want to relocate!"));
		}
	}
}

void UProceduralLimbManager::AutoDetectLimbs(UPoseableMeshComponent* InMesh) {
	if(!InMesh) return;
	Mesh = InMesh;
	Limbs.Empty();

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
			auto LimbName = boneAsString;
			LimbName.RemoveAt(foundTip, templateName.Len());
			LimbName.RemoveFromEnd("_end");
			
			Limbs.Add(NewObject<ULimb>(this, ULimb::StaticClass(), *LimbName));

			
			Limbs.Last()->Bones.Add(FLimbSegment(Bone, FRotator::ZeroRotator));
			Limbs.Last()->IKTarget = Cast<USceneComponent>(
				GetOwner()->AddComponentByClass(USceneComponent::StaticClass(),
					true,
					FTransform::Identity,
					false));
			Limbs.Last()->IKTarget->SetWorldLocation(Mesh->GetBoneLocationByName(Bone, EBoneSpaces::WorldSpace));
			Limbs.Last()->RestingTargetLocation = Mesh->GetBoneLocationByName(Bone, EBoneSpaces::ComponentSpace);
			
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
			//Legs.Last().IKTarget->AttachToComponent(GetOwner()->GetRootComponent(), AttachmentRules);
			
			auto IKTargetName = FString::Printf(TEXT("IK_"));
			IKTargetName.AppendChars(*boneAsString, boneAsString.Len());
			Limbs.Last()->IKTarget->Rename(*IKTargetName);
			GetOwner()->AddInstanceComponent(Limbs.Last()->IKTarget);
			//Legs.Last().IKTarget = Mesh->GetBoneLocationByName(Bone, EBoneSpaces::WorldSpace);
			RecurseToHip(Bone);
		}
	}
	for(auto& Leg : Limbs) {
		for(int i = 1; i < Leg->Bones.Num(); ++i) {
			FVector Displacement = Mesh->GetBoneLocationByName(Leg->Bones[i-1].Name, EBoneSpaces::ComponentSpace);
				- Mesh->GetBoneLocationByName(Leg->Bones[i].Name, EBoneSpaces::ComponentSpace);
			Leg->Bones[i].Length = Displacement.Length();
		}
	}
}

void UProceduralLimbManager::RecurseToHip(FName From) {
	
	FName Parent = Mesh->GetParentBone(From);
	if(Parent.IsNone())
		return;
	Limbs.Last()->Bones.Add(
		FLimbSegment(Parent, Mesh->GetBoneRotationByName(Parent, EBoneSpaces::ComponentSpace)));
	auto boneAsString = Parent.ToString();
	auto templateName = HipJointsName.ToString();
	auto s = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
	if(s == INDEX_NONE)
		RecurseToHip(Parent);
}
