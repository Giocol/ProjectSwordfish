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
		Limbs[i]->UpdateIK(Mesh,IKThreshold, IKIterations, true);
		// FVector Displacement;
		// if(Limbs[i]->PrefersTargetRelocation(Mesh, 15.f, Displacement)) {
		// 	FVector ComponentSpaceLocation = Limbs[i]->RestingTargetLocation - 1 * Displacement;
		// 	FVector WorldSpaceLocation = Mesh->GetComponentTransform().TransformPosition(ComponentSpaceLocation);
		// 	Limbs[i]->IKTarget->SetWorldLocation(WorldSpaceLocation);
		// 	GEngine->AddOnScreenDebugMessage(i, .1f, FColor::Emerald, TEXT("I want to relocate!"));
		// }
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

			auto Limb = NewObject<ULimb>(this, ULimb::StaticClass(), *LimbName);
			Limb->Initialize(Mesh, Bone, HipJointsName);
			Limbs.Add(Limb);
		}
	}
	// for(auto& Leg : Limbs) {
	// 	for(int i = 1; i < Leg->Bones.Num(); ++i) {
	// 		FVector Displacement = Mesh->GetBoneLocationByName(Leg->Bones[i-1].Name, EBoneSpaces::ComponentSpace);
	// 			- Mesh->GetBoneLocationByName(Leg->Bones[i].Name, EBoneSpaces::ComponentSpace);
	// 		Leg->Bones[i].Length = Displacement.Length();
	// 	}
	// }
}
