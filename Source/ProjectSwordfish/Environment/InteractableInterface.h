#pragma once
#include "InteractableInterface.generated.h"

class AMainCharacter;

UINTERFACE()
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void Interact(AMainCharacter* InteractionInstigator);
};

inline void IInteractableInterface::Interact(AMainCharacter* InteractionInstigator)
{
}
