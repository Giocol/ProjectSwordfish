#include "FishingQTEHandler.h"

#include "ProjectSwordfish/DataAssets/FishingEventDataAsset.h"
#include "ProjectSwordfish/DataAssets/FishingQuickTimeEventDataAsset.h"


UFishingQTEHandler::UFishingQTEHandler() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UFishingQTEHandler::StartQTEs(UFishingEventDataAsset* FishingEventIn, std::function<void(bool)> OnAllQTEsResolvedCallback, std::function<void(void)> OnQTEStartedCallback, std::function<void(void)> OnQTEEndedCallback) {
	FishingEvent = FishingEventIn;
	OnAllQTEsResolved = OnAllQTEsResolvedCallback;
	OnQTEStart = OnQTEStartedCallback;
	OnQTEEnd = OnQTEEndedCallback;
	
	if(FishingEvent->QTEData.Num() > 0) {
		CurrentQTE = FishingEvent->QTEData[0];
		InitializeCurrentQTE();
	} else
		OnAllQTEsResolved(true);
}

void UFishingQTEHandler::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrentQTE)
		QTETick(DeltaTime);
}

void UFishingQTEHandler::InitializeCurrentQTE() {
	if(CurrentQTE->bDoesRepeat)
		CurrentQTENumberOfRepetitions = CurrentQTE->GetNumberOfRepetitions();
	else
		CurrentQTENumberOfRepetitions = 1;
	
	CurrentRepetitionIndex = 0;
	InitializeCurrentRepetition();
}

void UFishingQTEHandler::InitializeCurrentRepetition() {
	bIsWaitingForNextRepetition = false;
	NextRepetitionWaitTimeElapsed = 0.f;
	bHasQTEInputBeenDetected = false;
	
	CurrentRepetitionDirection = CurrentQTE->GetDirection();
	CurrentRepetitionTimeToComplete = CurrentQTE->GetTimeToComplete();
	NextRepetitionWaitTime = CurrentQTE->GetRepeatCooldown();
	CurrentRepetitionTimePressed = 0;
	
	OnQTEStart();
}

void UFishingQTEHandler::OnRepetitionCompleted() {
	if(CurrentRepetitionIndex == CurrentQTENumberOfRepetitions - 1)
		OnQTECompleted();
	else {
		CurrentRepetitionIndex++;
		bIsWaitingForNextRepetition = true;
	}

	OnQTEEnd();
}

void UFishingQTEHandler::OnQTECompleted() {
	if(CurrentQTEIndex == FishingEvent->QTEData.Num() - 1) {
		CurrentQTE = nullptr;
		FishingEvent = nullptr;
		OnAllQTEsResolved(true);
	}
	else {
		CurrentQTEIndex++;
		InitializeCurrentQTE();
	}
}

void UFishingQTEHandler::QTETick(float DeltaTime) {
	if(!bIsWaitingForNextRepetition) {
		if((CurrentRepetitionDirection == Left && bIsLeaningLeft)|| (CurrentRepetitionDirection == Right && bIsLeaningRight)) {
			CurrentRepetitionTimePressed += DeltaTime;
		}
		else
			CurrentRepetitionTimePressed = FMath::Clamp(CurrentRepetitionTimePressed - DeltaTime, 0, CurrentRepetitionTimePressed);

		if(bIsLeaningLeft || bIsLeaningRight)
			bHasQTEInputBeenDetected = true;
			
		if(CurrentRepetitionTimePressed >= CurrentRepetitionTimeToComplete)
			OnRepetitionCompleted();
	} else {
		NextRepetitionWaitTimeElapsed += DeltaTime;
		if(NextRepetitionWaitTimeElapsed >= NextRepetitionWaitTime)
			InitializeCurrentRepetition();
	}

	if(CurrentRepetitionTimePressed == 0 && bHasQTEInputBeenDetected) //ensures that QTE can only be failed after the player pressed the corresponding input to guarantee more fairness
		OnQTEFailure();
}

void UFishingQTEHandler::OnQTEFailure() {
	CurrentQTE = nullptr;
	FishingEvent = nullptr;
	OnQTEEnd();
	OnAllQTEsResolved(false);
}

