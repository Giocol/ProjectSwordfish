#pragma once

#include "CoreMinimal.h"
#include "Limb.generated.h"

class ULimbSegment;
class UPoseableMeshComponent;

UCLASS(Blueprintable)
class ULimb : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleInstanceOnly)
	TArray<ULimbSegment*> Bones;
	UPROPERTY(VisibleInstanceOnly)
	FVector IKTarget;
	FVector RestingTargetLocation;
	
	UPROPERTY(VisibleInstanceOnly)
		bool bIsRelocating = false;

	void UpdateIK(UPoseableMeshComponent* Mesh, bool bDraw = false);
	bool PrefersTargetRelocation(UPoseableMeshComponent* Mesh, float MaxDistance, FVector& Displacement);

	bool Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor);
protected:
	ULimbSegment* MakeNode(UPoseableMeshComponent* Mesh, FName BoneName, FName HipNameToSearchFor, bool bIsRoot = false);
	
	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);

	
	bool CCDIK_SmartBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	int CCDIK_BackwardBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	bool JacobianIK_PseudoInverse(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);
	
	void ApplyBoneTransformation(UPoseableMeshComponent* Mesh);
	void DrawIK(UPoseableMeshComponent* Mesh);

	FQuat GetRotatorBetween(FVector ToEnd, FVector ToTarget);
	FQuat GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
};
