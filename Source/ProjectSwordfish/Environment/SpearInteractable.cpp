#include "SpearInteractable.h"

#include "ProjectSwordfish/Player/MainCharacter.h"


ASpearInteractable::ASpearInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	SpearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spear Mesh"));
	SpearMesh->SetupAttachment(RootComponent);
}

void ASpearInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpearInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpearInteractable::Interact(AMainCharacter* InteractionInstigator) {
	IInteractableInterface::Interact(InteractionInstigator);
	
	UE_LOG(LogTemp, Warning, TEXT("Player tried to interact with me"))
	InteractionInstigator->SetHasSpear(true);
	SpearMesh->SetVisibility(false);
}

