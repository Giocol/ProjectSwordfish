#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FishingQTEHandler.generated.h"


enum EQTEDirection : int;
class UFishingQuickTimeEventDataAsset;
class UFishingEventDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UFishingQTEHandler : public UActorComponent {
	GENERATED_BODY()

public:
	UFishingQTEHandler();

	void StartQTEs(UFishingEventDataAsset* FishingEventIn, std::function<void(void)> OnQTEsResolvedCallback);
	
	virtual void SetIsLeaningLeft(bool State) { bIsLeaningLeft = State; }
	virtual void SetIsLeaningRight(bool State) { bIsLeaningRight = State; }
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void InitializeCurrentQTE();
	void InitializeCurrentRepetition();
	void OnRepetitionCompleted();
	void OnQTECompleted();
	void QTETick(float DeltaTime);
	
protected:
	UFishingEventDataAsset* FishingEvent = nullptr;
	std::function<void(void)> OnQTEsResolved = nullptr;
	
	UFishingQuickTimeEventDataAsset* CurrentQTE = nullptr;
	int CurrentQTEIndex = 0;

	int CurrentQTENumberOfRepetitions = 0;
	int CurrentRepetitionIndex = 0;
	EQTEDirection CurrentRepetitionDirection;
	float CurrentRepetitionTimePressed = 0.f;
	float CurrentRepetitionTimeToComplete = 0.f;
	
	bool bIsLeaningLeft = false;
	bool bIsLeaningRight = false;
};
