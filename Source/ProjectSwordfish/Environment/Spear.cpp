#include "Spear.h"


ASpear::ASpear()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpear::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpear::Interact(AMainCharacter* InteractionInstigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Player tried to interact with me"))
}

