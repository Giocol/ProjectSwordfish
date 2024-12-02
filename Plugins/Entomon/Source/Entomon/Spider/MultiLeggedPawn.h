// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entomon/Navigation/PathPreference.h"
#include "GameFramework/Pawn.h"
#include "Entomon/Navigation/NavNode.h"
#include "Entomon/Navigation/PathNode.h"

#include "MultiLeggedPawn.generated.h"

class UGaitPreset;
class UFloatingPawnMovement;
class UMovementComponent;
class UProceduralLimbManager;

UENUM()
enum class EPathSmoothingType : uint8 {
	None = 0,
	Uniform,
	Normal
};
	
UCLASS()
class ENTOMON_API AMultiLeggedPawn : public APawn
{
	GENERATED_BODY()

public:
	AMultiLeggedPawn();
	
	UFUNCTION(BlueprintCallable)
	void SetPath(TArray<FNavNode> Nodes);

	class UPoseableMeshComponent* GetMesh() { return Mesh; }

	USceneComponent* GetBody() const { return Body; }

	FVector GetAngularVelocity() const { return AngularVelocity; }

protected:
	virtual void BeginPlay() override;

	bool Move(double DeltaTime, int Target);
	void Rotate(double DeltaTime, int Target);
	void FollowPath(double DeltaTime);
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ApplyGaitPreset(UGaitPreset* InGaitPreset);

private:
	bool Trace(FVector Start, FVector Direction, FHitResult& OutHit);
	TArray<FHitResult> FibonacciTrace(FVector Start);
	FHitResult GetClosestWhisker(TArray<FHitResult> Hits, bool bDraw);

	float GetNormalizedOffsetFromPreference(float Distance);
	float GetDistanceFromNodePlane(int Node);
	FVector GetTangent(int AtId);
	void CorrectPath();
	void SimplifyPath();
	void SubdividePath();
	void SmoothPath();
	TArray<FPathNode> SmoothMovingAverage(TArray<FPathNode> InPath) const;

	float GetNormalizedInterpolatorToNextNode();

	FVector GetBobbingImpulse();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		UFloatingPawnMovement* MovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", UIMin = 0.0001f, UIMax = 1.f))
		float FacingBias = 0.1f; // Defines the importance of facing the target the pawn wants to approach
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float RotationSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		UGaitPreset* GaitPreset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		FPathPreference PathPreference;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		int NumWhiskers = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float WhiskerRayDistance = 500;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float PreferredPersonalSpace = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float MinDistanceFromObstacle = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float MaxDistanceFromObstacle = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", EditCondition="PathSmoothingType!=EPathSmoothingType::None", EditConditionHides, UIMin=0, ToolTip="How far the smoothing is applied, where 1 just s"))
		bool bCorrectPath = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		EPathSmoothingType PathSmoothingType = EPathSmoothingType::Uniform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", EditCondition="PathSmoothingType!=EPathSmoothingType::None", EditConditionHides, UIMin=0, ToolTip="How far the smoothing is applied, where 1 just s"))
		int PathSmoothing = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", EditCondition="PathSmoothingType!=EPathSmoothingType::None", EditConditionHides, UIMin=0, ToolTip="How far the smoothing is applied, where 1 just s"))
		bool bCurveCorrection = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", InlineEditConditionToggle))
		bool bSimplify = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", EditCondition="bSimplify", UIMin = 0.f, UIMax=1.f))
		float Simplify = 0.1f;
	TArray<FPathNode> Path;
	int CurrentPathId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gizmos", meta=(AllowPrivateAccess="true", InlineEditConditionToggle))
		bool bDrawPath = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		UProceduralLimbManager* LimbManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		class UPoseableMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		class USceneComponent* Body;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		USceneComponent* Root;

	FVector AngularVelocity = FVector::ZeroVector;
};
