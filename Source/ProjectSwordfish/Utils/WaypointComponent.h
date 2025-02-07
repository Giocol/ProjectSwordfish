#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WaypointComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UWaypointComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UWaypointComponent();

	void SetColor(const FColor NewColor) { Color = NewColor;};

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY()
	FColor Color = FColor::Purple;
};
