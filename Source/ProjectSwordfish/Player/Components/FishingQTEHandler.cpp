#include "FishingQTEHandler.h"

#include "ProjectSwordfish/DataAssets/FishingEventDataAsset.h"
#include "ProjectSwordfish/DataAssets/FishingQuickTimeEventDataAsset.h"


UFishingQTEHandler::UFishingQTEHandler() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UFishingQTEHandler::StartQTEs(UFishingEventDataAsset* FishingEventIn) {
	FishingEvent = FishingEventIn;

	HandleNextQTE();
}

void UFishingQTEHandler::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrentQTE)
		QTETick(DeltaTime);
}

void UFishingQTEHandler::HandleNextQTE() { 
	CurrentQTE = FishingEvent->QTEData[0]; // this is ofc just for testing

	
}

void UFishingQTEHandler::QTETick(float DeltaTime) {
	//todo: call getdirection only once per repetetion
	if((CurrentQTE->GetDirection() == Left && bIsLeaningLeft) || (CurrentQTE->GetDirection() == Right && bIsLeaningRight))
		CurrentQTETime += DeltaTime;

	if(CurrentQTETime >= CurrentQTE->GetTimeToComplete()) {
		// on qte resolved TODO: add the repetitions n shit
	}
}

