#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FishingQTEHandler.generated.h"


class UFishingQuickTimeEventDataAsset;
class UFishingEventDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UFishingQTEHandler : public UActorComponent {
	GENERATED_BODY()

public:
	UFishingQTEHandler();

	void StartQTEs(UFishingEventDataAsset* FishingEventIn);
	
	virtual void SetIsLeaningLeft(bool State) { bIsLeaningLeft = State; }
	virtual void SetIsLeaningRight(bool State) { bIsLeaningRight = State; }
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void HandleNextQTE();
	void QTETick(float DeltaTime);
	
protected:
	UFishingEventDataAsset* FishingEvent = nullptr;
	UFishingQuickTimeEventDataAsset* CurrentQTE = nullptr;
	int CurrentQTEIndex = 0;

	float CurrentQTETime = 0.f;
	
	bool bIsLeaningLeft = false;
	bool bIsLeaningRight = false;
};
