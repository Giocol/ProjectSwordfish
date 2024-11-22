#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FishingQuickTimeEventDataAsset.generated.h"

UCLASS()
class PROJECTSWORDFISH_API UFishingQuickTimeEventDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "General")
		FName Name = "Fishing QTE";

	UPROPERTY(EditDefaultsOnly, Category = "General")
		bool bIsTimeToCompleteRandomized = false;

	UPROPERTY(EditDefaultsOnly, Category = "General")
		bool bDoesRepeat = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "General", meta = (EditCondition="bDoesRepeat", EditConditionHides))
		bool bIsRepeatCooldownRandomized = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Thresholds", meta = (ClampMin = "0", UIMin = "0", EditCondition="bIsTimeToCompleteRandomized", EditConditionHides))
		float TimeToCompleteLowerThreshold = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Thresholds", meta = (ClampMin = "0", UIMin = "0"))
		float TimeToComplete = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Thresholds", meta = (ClampMin = "0", UIMin = "0", EditCondition="bDoesRepeat && bIsRepeatCooldownRandomized", EditConditionHides))
		float RepeatCooldownLowerThreshold = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Thresholds", meta = (ClampMin = "0", UIMin = "0", EditCondition="bDoesRepeat", EditConditionHides))
		float RepeatCooldown = 0;

public:
	float GetTimeToComplete() const { return bIsTimeToCompleteRandomized ? TimeToComplete : FMath::RandRange(TimeToCompleteLowerThreshold, TimeToComplete); }
	float GetRepeatCooldown() const { return bIsRepeatCooldownRandomized ? RepeatCooldownLowerThreshold : FMath::RandRange(RepeatCooldownLowerThreshold, RepeatCooldown); }
	
#if WITH_EDITOR // Just for editor stuff
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

#if WITH_EDITOR // Just for editor stuff, clamps sanityMin and Max relative to eachtoher 
inline void UFishingQuickTimeEventDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);   

	TimeToCompleteLowerThreshold = FMath::Clamp(TimeToCompleteLowerThreshold, 0, TimeToComplete);
	RepeatCooldownLowerThreshold = FMath::Clamp(RepeatCooldownLowerThreshold, 0, RepeatCooldown);
}
#endif
