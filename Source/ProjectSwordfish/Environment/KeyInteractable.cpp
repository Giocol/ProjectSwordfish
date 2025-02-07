#include "KeyInteractable.h"

void AKeyInteractable::Interact(AMainCharacter* InteractionInstigator) {
	IInteractableInterface::Interact(InteractionInstigator);

	PickupInstigator = InteractionInstigator;
	
}
