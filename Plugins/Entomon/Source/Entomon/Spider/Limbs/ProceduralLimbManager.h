// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Limb.h"

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

	TArray<ULimb*> GetLimbs() { return Limbs; }
	void ApplyGaitPreset(UGaitPreset* GaitPreset);
	
	void AutoDetectLimbs(UPoseableMeshComponent* InMesh);
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		TArray<ULimb*> Limbs;
	float WalkCycleCounter = 0.f;
	
	float WalkCycleDuration = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "IK Solver")
		int IKIterations = 10;
	UPROPERTY(EditAnywhere, Category = "IK Solver")
		float IKThreshold = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Auto Limb Finder")
		FName HipJointsName = "Hip";
	UPROPERTY(EditDefaultsOnly, Category = "Auto Limb Finder")
		FName EndEffectorsName = "Tip";
	UPROPERTY(EditDefaultsOnly, Category = "Auto Limb Finder")
		bool bUsesPoles = true;
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bUsesPoles"))
		FName PoleName = "Pole";

	UPROPERTY()
	class UPoseableMeshComponent* Mesh;
};


