#include "FuzzyBrainComponent.h"


UFuzzyBrainComponent::UFuzzyBrainComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UFuzzyBrainComponent::BeginPlay() {
	Super::BeginPlay();
}


void UFuzzyBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

