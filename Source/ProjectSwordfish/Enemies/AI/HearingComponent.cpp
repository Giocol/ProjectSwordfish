#include "HearingComponent.h"


UHearingComponent::UHearingComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UHearingComponent::BeginPlay() {
	Super::BeginPlay();
}

void UHearingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

