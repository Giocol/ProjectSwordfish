#include "USliderWithTargetAreas.h"

#include "Components/Image.h"
#include "Components/NamedSlot.h"
#include "ProjectSwordfish/Player/MainCharacter.h"

void UUSliderWithTargetAreas::NativeConstruct() {
	Super::NativeConstruct();
	playerRef = Cast<AMainCharacter>(GetOwningPlayerPawn());
}

void UUSliderWithTargetAreas::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(!playerRef)
		return;

	float HandleValue = playerRef->GetCurrentAim();
}

float UUSliderWithTargetAreas::NormalizeXCoords(float XCoords) const {
	return  XCoords/SliderBar->GetDesiredSize().X;
}
