// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GaitPreset.generated.h"

USTRUCT(Blueprintable)
struct FKeywordOption {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FName> Names;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(InlineEditConditionToggle))
		bool bUseLocator = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bUseLocator"))
		TArray<FName> Locators;
};

USTRUCT(Blueprintable)
struct FGait {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FKeywordOption Keywords;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (UIMin=0.f, UIMax=1.f))
		float GaitOffset = 0.f;
};

UCLASS(BlueprintType, CollapseCategories)
class ENTOMON_API UGaitPreset : public UDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0.001))
		float WalkCycleDuration = 0.5f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0.001))
		float MaxWalkCycleDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
		float StepDuration = 0.1f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
		float StepHeight = 10.f;
	UPROPERTY(EditDefaultsOnly)
		float MaxSpeed = 500.f;
	UPROPERTY(EditDefaultsOnly)
		float Acceleration = 2000.f;
	UPROPERTY(EditDefaultsOnly)
		float Deceleration = 2000.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0.f))
		float BobbingImpulse = 1.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0.f))
		float TakeoffAngularImpulse = 1.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0.f))
		float LandingAngularImpulse = 1.f;
	UPROPERTY(EditDefaultsOnly)
		TArray<FGait> PerLimbGaitInfo;
};
