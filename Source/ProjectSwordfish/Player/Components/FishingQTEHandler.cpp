#include "FishingQTEHandler.h"

#include "ProjectSwordfish/DataAssets/FishingEventDataAsset.h"
#include "ProjectSwordfish/DataAssets/FishingQuickTimeEventDataAsset.h"


UFishingQTEHandler::UFishingQTEHandler() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UFishingQTEHandler::StartQTEs(UFishingEventDataAsset* FishingEventIn, std::function<void()> OnQTEsResolvedCallback, std::function<void()> OnQTEStartedCallback) {
	FishingEvent = FishingEventIn;
	OnQTEsResolved = OnQTEsResolvedCallback;
	OnQTEStart = OnQTEStartedCallback;
	
	if(FishingEvent->QTEData.Num() > 0) {
		CurrentQTE = FishingEvent->QTEData[0];
		InitializeCurrentQTE();
	} else
		OnQTEsResolved();
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
	CurrentRepetitionDirection = CurrentQTE->GetDirection();
	CurrentRepetitionTimeToComplete = CurrentQTE->GetTimeToComplete();

	OnQTEStart();
}

void UFishingQTEHandler::OnRepetitionCompleted() {
	if(CurrentRepetitionIndex == CurrentQTENumberOfRepetitions - 1)
		OnQTECompleted();
	else {
		CurrentRepetitionIndex++;
		InitializeCurrentRepetition();
	}
}

void UFishingQTEHandler::OnQTECompleted() {
	if(CurrentQTEIndex == FishingEvent->QTEData.Num() - 1) {
		CurrentQTE = nullptr;
		FishingEvent = nullptr;
		OnQTEsResolved();
	}
	else {
		CurrentQTEIndex++;
		InitializeCurrentQTE();
	}
}

void UFishingQTEHandler::QTETick(float DeltaTime) {
	if((CurrentRepetitionDirection == Left && bIsLeaningLeft)|| (CurrentRepetitionDirection == Right && bIsLeaningRight))
		CurrentRepetitionTimePressed += DeltaTime;
	
	if(CurrentRepetitionTimePressed >= CurrentRepetitionTimeToComplete) {
		OnRepetitionCompleted();
	}
}

