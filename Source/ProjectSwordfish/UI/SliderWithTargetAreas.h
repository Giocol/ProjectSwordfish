#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderWithTargetAreas.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;
class AMainCharacter;
class UImage;

UENUM()
enum EFishingSliderType
{
	None = 0,
	Aiming,
	Power
};

UCLASS()
class PROJECTSWORDFISH_API UUSliderWithTargetAreas : public UUserWidget {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateHandlePosition();
	void SetTargetAreasPosition();

protected:
	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<EFishingSliderType> Type = Aiming; 
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCanvasPanel* CanvasPanel = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* SliderBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* GoodTargetArea = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* Handle = nullptr;

private:
	AMainCharacter* playerRef = nullptr;
	
	UCanvasPanelSlot* SliderBarPanelSlot = nullptr;
	UCanvasPanelSlot* GoodAreaPanelSlot = nullptr;
	UCanvasPanelSlot* HandlePanelSlot = nullptr;

};
