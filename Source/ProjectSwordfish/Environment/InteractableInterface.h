#pragma once
#include "InteractableInterface.generated.h"

class AMainCharacter;

UINTERFACE(Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnInteract();
	
	UFUNCTION()
		virtual void Interact(AMainCharacter* InteractionInstigator) {
			Execute_OnInteract(this->_getUObject()); //TODO: THIS IS TERRIFYING WHY DOES IT WORK
		};
};
