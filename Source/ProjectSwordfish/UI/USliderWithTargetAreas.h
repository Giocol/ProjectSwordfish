#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USliderWithTargetAreas.generated.h"

class AMainCharacter;
class UImage;

UCLASS()
class PROJECTSWORDFISH_API UUSliderWithTargetAreas : public UUserWidget {
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float NormalizeXCoords(float XCoords) const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* SliderBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* GoodTargetArea = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNamedSlot* Handle = nullptr;

private:
	AMainCharacter* playerRef = nullptr;
};
