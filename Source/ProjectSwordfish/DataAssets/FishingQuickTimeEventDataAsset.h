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
		bool bIsRadiusRandomized = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Thresholds", meta = (ClampMin = "0", UIMin = "0", EditCondition="bIsRadiusRandomized", EditConditionHides))
		float RadiusLowerThreshold = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Thresholds", meta = (ClampMin = "0", UIMin = "0"))
		float Radius = 0;

	UPROPERTY(EditDefaultsOnly, Category = "General")

public:
	float GetRadius() const { return bIsRadiusRandomized ? Radius : FMath::RandRange(RadiusLowerThreshold, Radius); }

#if WITH_EDITOR // Just for editor stuff
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

#if WITH_EDITOR // Just for editor stuff, clamps sanityMin and Max relative to eachtoher 
inline void UFishingQuickTimeEventDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);   

	RadiusLowerThreshold = FMath::Clamp(RadiusLowerThreshold, 0, Radius);
}
#endif
