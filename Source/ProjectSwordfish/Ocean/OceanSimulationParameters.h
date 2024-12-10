// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OceanSimulationParameters.generated.h"

DECLARE_DELEGATE_OneParam(FReadCompleteDelegate, TArray<FVector> /* OutVectors */);

USTRUCT(Blueprintable, BlueprintType)
struct FPerCascadeParameters {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters", meta = (AllowPreserveRatio))
		FVector4 WindDirectionality;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters", meta = (AllowPreserveRatio))
		FVector4 Amplitude;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters", meta = (AllowPreserveRatio))
		FVector4 Choppiness;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters", meta = (AllowPreserveRatio))
		FVector4 PatchLength;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters")
		FVector4 ShortWaveCutoff;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters")
		FVector4 LongWaveCutoff;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Parameters", meta = (AllowPreserveRatio))
		FVector4 WindTighten;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Foam Parameters", meta = (AllowPreserveRatio))
		FVector4 FoamInjection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Foam Parameters", meta = (AllowPreserveRatio))
		FVector4 FoamThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Foam Parameters", meta = (AllowPreserveRatio))
		FVector4 FoamFade;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Foam Parameters", meta = (AllowPreserveRatio))
		FVector4 FoamBlur;
};

USTRUCT(Blueprintable, BlueprintType)
struct FOceanInfo {
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wind Control")
		float WindSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wind Control", meta = (UIMin = 0.f, UIMax = 360.f))
		float WindDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roughness", meta = (SliderExponent = 2, UIMin = 0, UIMax = 1024))
		int RoughnessSamples;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roughness")
		float RoughnessPower;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc")
		float Gravity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc")
		float RepeatPeriod;
};