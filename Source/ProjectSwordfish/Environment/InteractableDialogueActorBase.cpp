#include "InteractableDialogueActorBase.h"


AInteractableDialogueActorBase::AInteractableDialogueActorBase() {
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractableDialogueActorBase::BeginPlay() {
	Super::BeginPlay();
}

void AInteractableDialogueActorBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AInteractableDialogueActorBase::Interact(AMainCharacter* InteractionInstigator) {
	IInteractableInterface::Interact(InteractionInstigator);
}

