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
	TArray<ULimbSegment*> Joints;
	UPROPERTY(VisibleInstanceOnly)
	FTransform IKTarget;
	FVector RestingTargetLocation;
	FVector HipLocation;
	float MaxLength = 0.f;
	
	UPROPERTY(VisibleInstanceOnly)
		bool bIsRelocating = false;

	bool Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor);
	void UpdateIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw = false);
	bool PrefersTargetRelocation(UPoseableMeshComponent* Mesh, float MaxDistance, FVector& Displacement);

	void ResetStates(UPoseableMeshComponent* Mesh);
protected:
	ULimbSegment* MakeNode(UPoseableMeshComponent* Mesh, FName BoneName, bool bIsRoot = false);
	
	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);

	
	int Solve_CCDIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);
	int Solve_FABRIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);

	void InitializeIK(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);

	void BackwardReach(UPoseableMeshComponent* Mesh, FVector ComponentSpaceTarget, TArray<FVector>& JointLocations);
	void ForwardReach(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);
	void EvaluateAngles(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);

	FVector GetJointLocation(int Id, TArray<FVector> JointLocations);

	//FVector GetPoleLocation(UPoseableMeshComponent* Mesh);
	FVector GetPoleNormal(UPoseableMeshComponent* Mesh, int Id);
	FVector GetPoleAxisVector(UPoseableMeshComponent* Mesh);
	FVector GetPoleAxisVector_World(UPoseableMeshComponent* Mesh);
	void CorrectPoles(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);
	
	void ApplyBoneTransformation(UPoseableMeshComponent* Mesh);
	void DrawIK(UPoseableMeshComponent* Mesh, float Threshold);

	FQuat GetRotatorBetween(FVector ToEnd, FVector ToTarget);
	FQuat GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
};
