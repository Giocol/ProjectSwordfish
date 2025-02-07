#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WaypointHolderComponent.generated.h"


class UWaypointComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UWaypointHolderComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UWaypointHolderComponent();
	
	void DrawPath(bool bPersistantLines = false);

	FVector GetNextWaypoint();

#if WITH_EDITOR
	void UpdateWaypointArray(const int NewSize, const FString& WaypointNamePrefix);

	void DeleteAllWaypoints();
#endif
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& FPropertyChangedEvent) override;

	protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditInstanceOnly, Category = "Waypoints")
	TArray<UWaypointComponent*> Waypoints;
	UPROPERTY(EditInstanceOnly, Category = "Waypoints")
	FColor DebugLineColor = FColor::Purple;

	int CurrentWaypointIndex = 0;
};
