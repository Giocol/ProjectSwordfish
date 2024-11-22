#include "SliderWithTargetAreas.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/NamedSlot.h"
#include "ProjectSwordfish/Player/MainCharacter.h"
#include "ProjectSwordfish/Player/UpstairsCharacter.h"

void USliderWithTargetAreas::NativeConstruct() {
	Super::NativeConstruct();
	playerRef = Cast<AUpstairsCharacter>(GetOwningPlayerPawn());
	SliderBarPanelSlot = Cast<UCanvasPanelSlot>(SliderBar->Slot);
	GoodAreaPanelSlot = Cast<UCanvasPanelSlot>(GoodTargetArea->Slot);
	HandlePanelSlot = Cast<UCanvasPanelSlot>(Handle->Slot);
	if(MediumTargetArea)
		MediumAreaPanelSlot = Cast<UCanvasPanelSlot>(MediumTargetArea->Slot);
	
	SetTargetAreasPosition();
}

void USliderWithTargetAreas::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(!playerRef || !SliderBarPanelSlot || !GoodAreaPanelSlot || !HandlePanelSlot)
		return;
	UpdateHandlePosition();
}


void USliderWithTargetAreas::UpdateHandlePosition() {
	float HandlePosition = playerRef->GetFishingSliderValue(Type) * SliderBarPanelSlot->GetSize().X;
	HandlePanelSlot->SetPosition(FVector2d(HandlePosition, HandlePanelSlot->GetPosition().Y));
}

void USliderWithTargetAreas::SetTargetAreasPosition() {
	if(!GoodAreaPanelSlot || !SliderBarPanelSlot)
		return;

	FFishingSliderData SliderData = playerRef->GetFishingSliderData(Type);
	float GoodTargetAreaWidth = (SliderData.GoodTargetAreaBounds.Y - SliderData.GoodTargetAreaBounds.X) * SliderBarPanelSlot->GetSize().X;
	float GoodTargetAreaPosition = SliderData.GoodTargetAreaBounds.X * SliderBarPanelSlot->GetSize().X;

	GoodAreaPanelSlot->SetSize(FVector2d(GoodTargetAreaWidth, SliderBarPanelSlot->GetSize().Y));
	GoodAreaPanelSlot->SetPosition(FVector2d(GoodTargetAreaPosition, SliderBarPanelSlot->GetPosition().Y));

	if(MediumAreaPanelSlot && SliderData.MediumTargetAreaBounds.X != -1 && SliderData.MediumTargetAreaBounds.Y != -1) {
		float MediumTargetAreaWidth = (SliderData.MediumTargetAreaBounds.Y - SliderData.MediumTargetAreaBounds.X) * SliderBarPanelSlot->GetSize().X;
		float MediumTargetAreaPosition = SliderData.MediumTargetAreaBounds.X * SliderBarPanelSlot->GetSize().X;

		MediumAreaPanelSlot->SetSize(FVector2d(MediumTargetAreaWidth, SliderBarPanelSlot->GetSize().Y));
		MediumAreaPanelSlot->SetPosition(FVector2d(MediumTargetAreaPosition, SliderBarPanelSlot->GetPosition().Y));
	}
}
