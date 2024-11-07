#pragma once
#include "SpearableInterface.generated.h"

class AMainCharacter;

UINTERFACE(Blueprintable)
class USpearableInterface : public UInterface
{
	GENERATED_BODY()
};

class ISpearableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
		virtual void OnSpeared(AMainCharacter* SpearingInstigator);

	UFUNCTION()
		virtual void OnPullCompleted(AMainCharacter* SpearingInstigator);
	
	UFUNCTION()
		virtual void OnUnSpeared(AMainCharacter* SpearingInstigator);
};

inline void ISpearableInterface::OnSpeared(AMainCharacter* SpearingInstigator) {
	UE_LOG(LogTemp, Warning, TEXT("OH NO I'VE BEEN SPEARED!"));
}

inline void ISpearableInterface::OnPullCompleted(AMainCharacter* SpearingInstigator) {
	UE_LOG(LogTemp, Warning, TEXT("I've been successfully pulled!"));
}

inline void ISpearableInterface::OnUnSpeared(AMainCharacter* SpearingInstigator) {
}
