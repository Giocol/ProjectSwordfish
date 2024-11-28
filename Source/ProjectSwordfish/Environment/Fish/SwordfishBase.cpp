#include "SwordfishBase.h"


ASwordfishBase::ASwordfishBase() {
	PrimaryActorTick.bCanEverTick = true;
}

void ASwordfishBase::BeginPlay() {
	Super::BeginPlay();
	
}

void ASwordfishBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

