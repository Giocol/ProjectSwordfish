#include "NoiseSystem.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectSwordfish/DataAssets/NoiseDataAsset.h"
#include "ProjectSwordfish/Enemies/AI/HearingComponent.h"
#include "Sound/SoundCue.h"

void ANoiseSystem::RegisterNoiseEvent(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location) {
	PlaySFX(NoiseDataAsset, Location);
	if(NoiseDataAsset && NoiseDataAsset->bCanAlertAI)
		DispatchNoiseEvent(NoiseDataAsset, Location);
}

void ANoiseSystem::DispatchNoiseEvent(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location) {
	for (const auto& Listener : Listeners) {
		Listener->OnNoiseHeard(NoiseDataAsset, Location);
	}
}

void ANoiseSystem::PlaySFX(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location, bool bPickRandom) const {
	if(!NoiseDataAsset || !NoiseDataAsset->Sound) {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Couldn't play sound, missing ref in NoiseDataAsset"));
		return;
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoiseDataAsset->Sound, Location);
}
