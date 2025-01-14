// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerceptionSignal.h"
#include "Components/ActorComponent.h"
#include "HearingComponent.generated.h"

class UNoiseDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UHearingComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UHearingComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	void OnNoiseHeard(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location);

	FPerceptionSignal GetLastNoiseSignal();
	
	bool HasNewSignalBeenHeard() const { return !bHasLastNoiseSignalBeenConsumed; };

private:
	UPROPERTY(VisibleAnywhere, Category = "Debug")
	float LastNoiseSignalIntesity = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	FVector LastNoiseSignalLocation;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	bool bHasLastNoiseSignalBeenConsumed = false;

	FPerceptionSignal LastNoiseSignal;
};
