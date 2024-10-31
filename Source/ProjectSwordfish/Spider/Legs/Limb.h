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
	FQuat State; // In component space.
	float Length = 0.f;
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
	
	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
private:
	bool CCDIK_SmartBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	bool CCDIK_BackwardBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	bool JacobianIK_PseudoInverse(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);
	
	void ApplyBoneTransformation(UPoseableMeshComponent* Mesh);
	void DrawIK(UPoseableMeshComponent* Mesh);

	FQuat GetRotatorBetween(FVector ToEnd, FVector ToTarget);
	FQuat GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
};
