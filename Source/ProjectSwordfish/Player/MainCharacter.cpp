#include "MainCharacter.h"

#include "Camera/CameraComponent.h"


AMainCharacter::AMainCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(RootComponent);
}

void AMainCharacter::ProcessCharacterMovementInput(const FVector2D input) {
	AddMovementInput(GetActorForwardVector(), input.X);
	AddMovementInput(GetActorRightVector(), input.Y);
}

void AMainCharacter::ProcessCameraMovementInput(const FVector2D Input) {
	AddControllerPitchInput(Input.X);
	AddControllerYawInput(Input.Y);
}

void AMainCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

void AMainCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

