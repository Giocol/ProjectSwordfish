#pragma once

#include "CoreMinimal.h"
#include "Limb.generated.h"

class UPoseableMeshComponent;

USTRUCT(Blueprintable)
struct FLimbSegment {
	GENERATED_BODY()
	FLimbSegment() {}
	FLimbSegment(FName Name, FRotator State)
		: Name(Name), State(State) { };

	UPROPERTY(VisibleInstanceOnly)
	FName Name;
	UPROPERTY(VisibleInstanceOnly)
	FRotator State; // In component space.
};
USTRUCT(Blueprintable)
struct FLeg {
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly)
	TArray<FLimbSegment> Bones; 
	UPROPERTY(VisibleInstanceOnly)
	USceneComponent* IKTarget;

	void Update(UPoseableMeshComponent* Mesh);
	
private:
	bool CCDIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw = false);
	void ApplyBoneTransformation(UPoseableMeshComponent* Mesh, bool bDraw = false);

	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
};
