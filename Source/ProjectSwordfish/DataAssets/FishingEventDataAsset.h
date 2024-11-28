#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FishingEventDataAsset.generated.h"

class ASwordfishBase;

UCLASS()
class PROJECTSWORDFISH_API UFishingEventDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "General")
		FName EventName = "Fishing Event";

	UPROPERTY(EditDefaultsOnly, Category = "General")
		ASwordfishBase* Swordfish = nullptr;
	
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

};
