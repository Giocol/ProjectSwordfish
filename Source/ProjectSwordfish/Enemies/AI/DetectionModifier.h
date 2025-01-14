#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DetectionModifier.generated.h"


UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UDetectionModifier : public UActorComponent {
	GENERATED_BODY()

public:
	UDetectionModifier();

	UFUNCTION()
	void SetDetectionSignalModifier(float Value);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0.f))
	float DefaultSignalModifier = 1.0;
};
