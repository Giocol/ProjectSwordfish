#pragma once
#include "CoreMinimal.h"

#include "PathPreference.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FPathPreference {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MinDistance = 0;
	UPROPERTY(EditAnywhere)
	float MaxDistance = INFINITY;
	UPROPERTY(EditAnywhere)
	float PreferredDistance = -1;
	UPROPERTY(EditAnywhere)
	float PreferenceWeight = 0.5;
	UPROPERTY(EditAnywhere)
	float BodySize = 50.f;
};
