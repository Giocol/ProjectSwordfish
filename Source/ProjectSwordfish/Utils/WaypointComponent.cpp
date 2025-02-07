#include "WaypointComponent.h"

UWaypointComponent::UWaypointComponent()
{
#if WITH_EDITOR
	PrimaryComponentTick.bCanEverTick = true;
#endif
}


void UWaypointComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									   FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), GetComponentLocation(), 20, 10, Color, false, -1, 0, 3);
#endif
}