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
	FVector RestVector;
};
USTRUCT(Blueprintable)
struct FLeg {
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly)
	TArray<FLimbSegment> Bones;
	UPROPERTY(VisibleInstanceOnly)
	TArray<FLimbSegment> RestPose; 
	UPROPERTY(VisibleInstanceOnly)
	USceneComponent* IKTarget;

	void Update(UPoseableMeshComponent* Mesh, bool bDraw = false);
	
private:
	bool CCDIK_SmartBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	void ApplyBoneTransformation(UPoseableMeshComponent* Mesh);
	void DrawIK(UPoseableMeshComponent* Mesh);

	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);

	FQuat GetRotatorBetween(FVector ToEnd, FVector ToTarget);
	FQuat GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
};
