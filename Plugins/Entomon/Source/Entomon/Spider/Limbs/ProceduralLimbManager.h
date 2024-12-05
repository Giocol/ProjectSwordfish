// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Limb.h"
#include "GameFramework/MovementComponent.h"

#include "ProceduralLimbManager.generated.h"

class UGaitPreset;

UCLASS(ClassGroup=(Locomotion), meta=(BlueprintSpawnableComponent))
class ENTOMON_API UProceduralLimbManager : public UActorComponent {
	GENERATED_BODY()

public:
	UProceduralLimbManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	void TickTimers(float DeltaTime, float& LastWalkCycleCounter);
	void TickLimbs(float DeltaTime, float LastWalkCycleCounter);
	
	TArray<ULimb*> GetLimbs() const { return Limbs; }
	void ApplyGaitPreset(UGaitPreset* GaitPreset);
	
	void AutoDetectLimbs(UPoseableMeshComponent* InMesh);
	
	FVector GetAverageLimbUpVector() const;
	void RegisterLimbLanding(ULimb* Limb);
	void RegisterLimbTakeoff(ULimb* Limb);
private:
	void ApproachLimbAverageRotation(double DeltaTime);

	float GetLerpedWalkCycleDuration();
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		TArray<ULimb*> Limbs;
	
	float WalkCycleCounter = 0.f;
	
	float WalkCycleDuration = -1;
	float MaxWalkCycleDuration = -1;

	float BobbingOffset = 0;
	
	UPROPERTY(EditAnywhere, Category = "IK Solver")
		int IKIterations = 10;
	UPROPERTY(EditAnywhere, Category = "IK Solver")
		float IKThreshold = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Auto Limb Finder")
		FName HipJointsName = "Hip";
	UPROPERTY(EditDefaultsOnly, Category = "Auto Limb Finder")
		FName EndEffectorsName = "Tip";
	UPROPERTY(EditAnywhere, Category = "Movement")
		float ToLimbAverageRotationSlerpSpeed = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Step Planning")
		float FootPlanningOriginZOffset = 100.f;
	UPROPERTY(EditAnywhere, Category = "Step Planning")
		int FootPlanningIterations = 16;
	UPROPERTY(EditDefaultsOnly, Category = "Step Planning")
		TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;
	

	UPROPERTY()
		class UPoseableMeshComponent* Mesh;

	class AMultiLeggedPawn* OwnerPawn;
	class UPawnMovementComponent* Movement;
	class UGaitPreset* GaitData;
};


