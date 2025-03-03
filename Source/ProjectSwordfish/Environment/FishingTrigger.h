﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishingTrigger.generated.h"

class ASwordfishBase;
class UFishingEventDataAsset;
class UBoxComponent;

UCLASS(Abstract)
class PROJECTSWORDFISH_API AFishingTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFishingTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
		UFishingEventDataAsset* FishingEventDataAsset = nullptr;

	UPROPERTY(EditAnywhere)
		ASwordfishBase* Swordfish = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
		UBoxComponent* Trigger = nullptr;

private:
	bool bHasBeenTriggered = false;
};
