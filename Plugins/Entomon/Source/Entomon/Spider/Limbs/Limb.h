#pragma once

#include "CoreMinimal.h"
#include "IKEffector.h"
#include "Limb.generated.h"

struct FLimbSegment;
class UPoseableMeshComponent;

UCLASS(Blueprintable)
class ULimb : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleInstanceOnly)
	TArray<FLimbSegment> Joints;
	
	FFootPlan FootPlan;
	
	float StepTimer = 0;
	float StepDuration = -1;
	float StepHeight = -1;
	bool bIsGrounded = true;
	
	FVector RestingTargetLocation;
	FVector HipLocation;
	float MaxLength = 0.f;
	
	float GaitOffset = -1;

	void Tick(UPoseableMeshComponent* Mesh, float DeltaTime, ECollisionChannel InTraceChannel);
	
	bool Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor);
	void UpdateIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw = false);
	bool TryMove(UPoseableMeshComponent* InMesh, float GaitCycleDuration, int Iterations,
		ECollisionChannel InTraceChannel);

	void ApplyGaitPreset(class UGaitPreset* InGaitPreset);
	void ResetStates(UPoseableMeshComponent* Mesh);
	
	static bool TraceFoot(ULimb* InLimb, UPoseableMeshComponent* Mesh, FVector InStart, FVector InDirection, float InDistance,
		ECollisionChannel InTraceChannel, FHitResult& OutHit);
	static bool TraceAround(ULimb* InLimb, UPoseableMeshComponent* Mesh, FVector InStart, FVector InDirection, float InDistance,
		int Iterations, ECollisionChannel InTraceChannel, FHitResult& OutHit);
protected:
	bool EvaluateTargetPosition(ULimb* InLimb, UPoseableMeshComponent* InMesh, float GaitCycleDuration, float TraceDistance, int Iterations,
								ECollisionChannel TraceChannel);
	FLimbSegment MakeJoint(UPoseableMeshComponent* Mesh, FName BoneName, bool bIsEnd = false);
	
	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	
	int Solve_FABRIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);
	void InitializeIK(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);

	void ResetRoll(UPoseableMeshComponent* Mesh);
	
	void BackwardReach(UPoseableMeshComponent* Mesh, FVector ComponentSpaceTarget, TArray<FVector>& JointLocations);
	void ForwardReach(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);
	void EvaluateAngles(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);

	FVector GetJointLocation(int Id, TArray<FVector> JointLocations);
	
	void DrawIK(UPoseableMeshComponent* Mesh, float Threshold);
};
