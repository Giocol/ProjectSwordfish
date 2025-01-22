#pragma once
#include "ProjectSwordfish/Systems/NoiseSystem.h"

class UNoiseDataAsset;

class PROJECTSWORDFISH_API INoiseMaker {
public:
	virtual ~INoiseMaker() = default;
	
protected:
	virtual void GenerateNoise(UNoiseDataAsset* NoiseDataAsset, FVector Location) {
		if(!NoiseSystemRef) {
			UE_LOG(
				LogTemp,
				Error,
				TEXT("MISSING NOISESYSTEMREF: the class that implements the INoiseMaker interface HAS to fetch NoiseSystemRef from GameMode"))
			return;
		}
		NoiseSystemRef->RegisterNoiseEvent(NoiseDataAsset, Location);
	}

	ANoiseSystem* NoiseSystemRef = nullptr;
};
