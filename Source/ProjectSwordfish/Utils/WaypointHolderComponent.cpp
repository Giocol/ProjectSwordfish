#include "WaypointHolderComponent.h"

#if WITH_EDITOR
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#endif

#include <string>

#include "WaypointComponent.h"


UWaypointHolderComponent::UWaypointHolderComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UWaypointHolderComponent::BeginPlay() {
	Super::BeginPlay();

	CurrentWaypointIndex = 0;
}


void UWaypointHolderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UWaypointHolderComponent::GetNextWaypoint() {
	CurrentWaypointIndex += 1;
	if(CurrentWaypointIndex >= Waypoints.Num())
		CurrentWaypointIndex = 0;

	if(Waypoints.Num() > 0)
		return Waypoints[CurrentWaypointIndex]->GetComponentLocation();
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s 's  WAYPOINT ARRAY IS EMPTY! IF THIS IS NOT INTENTIONAL, PLEASE FIX IT!"), *GetOwner()->GetName());
		return FVector::Zero();
	}
}

#if WITH_EDITOR
void UWaypointHolderComponent::DrawPath(bool bPersistantLines) {
	for(int i = 0; i < Waypoints.Num() - 1; i++) {
		DrawDebugLine(GetWorld(), Waypoints[i]->GetComponentLocation(), Waypoints[i+1]->GetComponentLocation(), DebugLineColor, bPersistantLines, -1, 0, 10);
	}
	if(Waypoints.Num() >= 2)
		DrawDebugLine(GetWorld(), Waypoints[Waypoints.Num() - 1]->GetComponentLocation(), Waypoints[0]->GetComponentLocation(), DebugLineColor, bPersistantLines, -1, 0, 10);
}


void UWaypointHolderComponent::UpdateWaypointArray(const int NewSize, const FString& WaypointNamePrefix) {
	for(int i = Waypoints.Num(); i < NewSize; i++) {
		FString WaypointName = WaypointNamePrefix + "Waypoint_" + FString::FromInt(i);
		UWaypointComponent* NewWaypoint = NewObject<UWaypointComponent>(GetOwner(), *WaypointName);
		NewWaypoint->SetWorldLocation(GetOwner()->GetActorLocation());
		NewWaypoint->SetupAttachment(this);
		GetOwner()->AddInstanceComponent(NewWaypoint);
		GetOwner()->AddOwnedComponent(NewWaypoint);
		NewWaypoint->RegisterComponent();
		NewWaypoint->SetColor(DebugLineColor);
		Waypoints.Add(NewWaypoint);
	}
}

void UWaypointHolderComponent::DeleteAllWaypoints() {
	for(int i = 0; i < Waypoints.Num(); i++) {
		Waypoints[i]->DestroyComponent();
	}
	Waypoints.Empty();
}

// void UWaypointHolderComponent::PostEditChangeProperty(FPropertyChangedEvent& FPropertyChangedEvent) {
// 	Super::PostEditChangeProperty(FPropertyChangedEvent);
// 	//GUnrealEd->UpdateFloatingPropertyWindows();
// }
#endif

