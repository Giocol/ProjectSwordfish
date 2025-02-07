#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoiseSystem.generated.h"

class UNoiseDataAsset;
class UHearingComponent;
//should be spawned by gamemode as a singleton
UCLASS(Abstract)
class PROJECTSWORDFISH_API ANoiseSystem : public AActor {
	GENERATED_BODY()
	
public:
	void RegisterListener(UHearingComponent* Listener) { Listeners.Add(Listener); };
	void UnregisterListener(UHearingComponent* Listener) { Listeners.Remove(Listener); };

	UFUNCTION(BlueprintCallable)
	void RegisterNoiseEvent(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location);
	
private:
	void DispatchNoiseEvent(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location);
	void PlaySFX(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location, bool bPickRandom = true) const;
	
private:
	UPROPERTY()
	TArray<UHearingComponent*> Listeners;
};
