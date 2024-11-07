#include "ProceduralLimbManager.h"

#include "Components/PoseableMeshComponent.h"
#include "Entomon/Spider/MultiLeggedPawn.h"
#include "Entomon/Spider/GaitPreset.h"

UProceduralLimbManager::UProceduralLimbManager() {
	//AutoDetectLimbs();
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UProceduralLimbManager::BeginPlay() {
	Super::BeginPlay();
}

void UProceduralLimbManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	float LastWalkCycleCounter = WalkCycleCounter;
	if(WalkCycleDuration != 0)
		WalkCycleCounter += DeltaTime / WalkCycleDuration;
	if(WalkCycleCounter > 1.f)
		WalkCycleCounter -= FMath::Floor(WalkCycleCounter);
	for(int i = 0; i < Limbs.Num(); ++i) {
		if((WalkCycleCounter > Limbs[i]->GaitOffset && LastWalkCycleCounter <= Limbs[i]->GaitOffset)
			|| (WalkCycleCounter >= Limbs[i]->GaitOffset && LastWalkCycleCounter > WalkCycleCounter)) {
			FVector RestingPositionWorld = Mesh->GetComponentTransform().TransformPosition(Limbs[i]->RestingTargetLocation);
			FVector ToRest = RestingPositionWorld - Limbs[i]->IKTarget.GetLocation();
			Limbs[i]->MoveTo(Limbs[i]->IKTarget.GetLocation() + 1.5 * ToRest, DeltaTime);
		}
		Limbs[i]->UpdateIK(Mesh, IKThreshold, IKIterations, true);
	}
}

void UProceduralLimbManager::ApplyGaitPreset(UGaitPreset* InGaitPreset) {
	WalkCycleDuration = InGaitPreset->WalkCycleDuration;
	for(auto Limb : Limbs) {
		bool bFoundMatch = true;
		for(auto Gait : InGaitPreset->PerLimbGaitInfo) {
			bFoundMatch = true;
			auto name = Limb->GetName();
			for(auto Keyword : Gait.Keywords) {
				auto r = Limb->GetName().Find(Keyword.ToString());
				if(r == INDEX_NONE) {
					bFoundMatch = false;
					break;
				}
			}
			if(bFoundMatch) {
				Limb->GaitOffset = Gait.GaitOffset;
				break;
			}
		}
		if(!bFoundMatch)
			UE_LOG(LogTemp, Error, TEXT("Error: Could not find limb with corresponding substrings"));
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
		int32 foundTip = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		int32 foundEnd = boneAsString.Find(*endName, ESearchCase::CaseSensitive);
		
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
