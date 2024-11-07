#pragma once

#include "CoreMinimal.h"
#include "Limb.generated.h"

struct FLimbSegment;
class UPoseableMeshComponent;

struct FIKTarget {
	FIKTarget() { }
	FIKTarget(FVector InLocation, FQuat InRotation)
		: Location(InLocation) {
		UpVector = InRotation.GetUpVector();
	}
	FIKTarget(FVector InLocation, FVector InUpVector)
		: Location(InLocation), UpVector(InUpVector) { }

	FVector GetLocation() const noexcept { return Location; }
	void SetLocation(const FVector& InLocation) noexcept { Location = InLocation; }

	FVector GetUpVector() const noexcept { return UpVector; }
	void SetUpVector(const FVector& InVector) noexcept { UpVector = InVector; }

	static FIKTarget Lerp(FIKTarget a, FIKTarget b, double alpha) {
		FIKTarget Result = a;
		Result.SetLocation(FMath::Lerp(a.GetLocation(), b.GetLocation(), alpha));
		Result.SetUpVector(FQuat::Slerp(a.GetUpVector().ToOrientationQuat(), b.GetUpVector().ToOrientationQuat(), alpha).GetForwardVector());
		return Result;
	}
	static FIKTarget LerpWithPeak(FIKTarget a, FIKTarget b, float StepHeight, double alpha) {
		FIKTarget Result = a;
		FIKTarget MidWay = Lerp(a, b, 0.5);
		float ParabolicAlpha = 4 * alpha - 4 * alpha * alpha;
		FVector PeakOffset = ParabolicAlpha * StepHeight * MidWay.GetUpVector();
		Result.SetLocation(FMath::Lerp(a.GetLocation(), b.GetLocation(), alpha) + PeakOffset);
		Result.SetUpVector(FQuat::Slerp(a.GetUpVector().ToOrientationQuat(), b.GetUpVector().ToOrientationQuat(), alpha).GetForwardVector());
		return Result;
	}
	
	FVector Location = FVector::ZeroVector;
	FVector UpVector = FVector::UpVector;
};

UCLASS(Blueprintable)
class ULimb : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleInstanceOnly)
	TArray<FLimbSegment> Joints;
	
	FIKTarget CurrentIK;
	FIKTarget StartIK;
	FIKTarget TargetIK;
	float StepTimer = 0;
	float StepDuration = -1;
	float StepHeight = -1;
	bool bIsGrounded = true;
	
	FVector RestingTargetLocation;
	FVector HipLocation;
	float MaxLength = 0.f;
	
	float GaitOffset = -1;

	void Tick(float DeltaTime);
	
	bool Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor);
	void UpdateIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw = false);
	bool TryMove(UPoseableMeshComponent* Mesh, float ZStartOffset, int Iterations,
		ECollisionChannel TraceChannel);

	void ApplyGaitPreset(class UGaitPreset* InGaitPreset);
	void ResetStates(UPoseableMeshComponent* Mesh);
protected:
	bool EvaluateTargetPosition(UPoseableMeshComponent* Mesh, float ZStartOffset, int Iterations,
								ECollisionChannel TraceChannel);
	FLimbSegment MakeJoint(UPoseableMeshComponent* Mesh, FName BoneName, bool bIsEnd = false);
	
	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	
	int Solve_CCDIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);
	int Solve_FABRIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);

	void InitializeIK(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations);

	void ResetRoll(UPoseableMeshComponent* Mesh);
	
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
