#pragma once

#include "CoreMinimal.h"
#include "Limb.generated.h"

class UPoseableMeshComponent;

USTRUCT(Blueprintable)
struct FLimbSegment {
	GENERATED_BODY()
	FLimbSegment() {}
	FLimbSegment(FName Name, FRotator State)
		: Name(Name), RestState(State), CurrentState(State) { };

	UPROPERTY(VisibleInstanceOnly)
	FName Name;
	UPROPERTY(VisibleInstanceOnly)
	FQuat RestState;
	UPROPERTY(VisibleInstanceOnly)
	FQuat CurrentState; // In component space.
	
	float Length = 0.f;
};

UCLASS(Blueprintable)
class ULimb : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleInstanceOnly)
	TArray<FLimbSegment> Bones;
	UPROPERTY(VisibleInstanceOnly)
	USceneComponent* IKTarget;
	FVector RestingTargetLocation;
	
	UPROPERTY(VisibleInstanceOnly)
		bool bIsRelocating = false;

	void UpdateIK(UPoseableMeshComponent* Mesh, bool bDraw = false);
	bool PrefersTargetRelocation(UPoseableMeshComponent* Mesh, float MaxDistance, FVector& Displacement);
protected:
	
	FVector GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
	FVector GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);

	
	bool CCDIK_SmartBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	bool CCDIK_BackwardBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance);
	bool JacobianIK_PseudoInverse(UPoseableMeshComponent* Mesh, float Threshold, int Iterations);
	
	void ApplyBoneTransformation(UPoseableMeshComponent* Mesh);
	void DrawIK(UPoseableMeshComponent* Mesh);

	FQuat GetRotatorBetween(FVector ToEnd, FVector ToTarget);
	FQuat GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace);
};
