#include "MainCharacter.h"


AMainCharacter::AMainCharacter() {
	PrimaryActorTick.bCanEverTick = true;
}

void AMainCharacter::ProcessCharacterMovementInput(FVector2D input) {
	//handle input
}

void AMainCharacter::ProcessCameraMovementInput(FVector2D input) const {
	//handle input
}

void AMainCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

void AMainCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

