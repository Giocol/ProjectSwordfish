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
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCurrentQTEProgress() const { return CurrentQTE ? CurrentRepetitionTimePressed / CurrentRepetitionTimeToComplete : 0; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		EQTEDirection GetCurrentQTEDirection() const { return CurrentRepetitionDirection; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsQTERunning() const { return CurrentQTE != nullptr; };
	
	void StartQTEs(UFishingEventDataAsset* FishingEventIn, std::function<void()> OnAllQTEsResolvedCallback, std::function<void()>
	               OnQTEStartedCallback, std::function<void()> OnQTEEndedCallback);

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
	std::function<void(void)> OnAllQTEsResolved = nullptr;
	std::function<void(void)> OnQTEStart = nullptr;
	std::function<void(void)> OnQTEEnd = nullptr;
	
	UFishingQuickTimeEventDataAsset* CurrentQTE = nullptr;
	int CurrentQTEIndex = 0;

	bool bIsWaitingForNextRepetition = false;
	float NextRepetitionWaitTimeElapsed = 0.f;
	float NextRepetitionWaitTime = 0.f;
	int CurrentQTENumberOfRepetitions = 0;
	int CurrentRepetitionIndex = 0;
	EQTEDirection CurrentRepetitionDirection;
	float CurrentRepetitionTimePressed = 0.f;
	float CurrentRepetitionTimeToComplete = 0.f;
	
	bool bIsLeaningLeft = false;
	bool bIsLeaningRight = false;
};
