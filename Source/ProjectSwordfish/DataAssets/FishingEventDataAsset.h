#pragma once

#include "CoreMinimal.h"
#include "FishingQuickTimeEventDataAsset.h"
#include "Engine/DataAsset.h"
#include "FishingEventDataAsset.generated.h"

class UFishingQuickTimeEventDataAsset;
class ASwordfishBase;

UCLASS()
class PROJECTSWORDFISH_API UFishingEventDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "General")
		FName EventName = "Fishing Event";

	UPROPERTY()
		ASwordfishBase* Swordfish = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "General")
		TArray<UFishingQuickTimeEventDataAsset*> QTEData;

	UPROPERTY(EditDefaultsOnly, Category = "General")
		float BaseTimeToPull = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target areas", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float AimLowerThreshold = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target areas",  meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float AimUpperThreshold = 0.4f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target areas", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float PowerGoodLowerThreshold = 0.45f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target areas",  meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float PowerGoodUpperThreshold = 0.55f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target areas", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float PowerMediumLowerThreshold = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target areas",  meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float PowerMediumUpperThreshold = 0.65f;

public:
	float GetTotalTimeToPull() const {
		float QTEsTime = 0.f;
		for (UFishingQuickTimeEventDataAsset* QTE : QTEData) {
			QTEsTime += (QTE->GetTimeToComplete() + QTE->GetRepeatCooldown()) * QTE->GetNumberOfRepetitions();
		}
		return QTEsTime + BaseTimeToPull;
	}
	
#if WITH_EDITOR // Just for editor stuff
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

#if WITH_EDITOR // Just for editor stuff, clamps sanityMin and Max relative to eachtoher 
inline void UFishingEventDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);   

	AimLowerThreshold = FMath::Clamp(AimLowerThreshold, 0, AimUpperThreshold);
	PowerGoodLowerThreshold = FMath::Clamp(PowerGoodLowerThreshold, 0, PowerGoodUpperThreshold);
	PowerMediumLowerThreshold = FMath::Clamp(PowerMediumLowerThreshold, 0, PowerMediumUpperThreshold);
}
#endif
