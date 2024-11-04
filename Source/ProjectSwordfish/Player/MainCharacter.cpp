#include "MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "ProjectSwordfish/Environment/InteractableInterface.h"


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

void AMainCharacter::ProcessInteract() {
	FHitResult HitResult;
	TraceInteract(HitResult);

	if(HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractableInterface>())
	{
		IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitResult.GetActor());
		Interactable->Interact(this);
	}
}

void AMainCharacter::ProcessUse() {
}

void AMainCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

void AMainCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AMainCharacter::TraceInteract(FHitResult& OutHitResult) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	FRotator StartRotation;
	FVector StartLocation;

	GetController()->GetPlayerViewPoint(StartLocation, StartRotation);
	const FVector EndLocation = StartLocation + StartRotation.Vector() * InteractionRange;

	GetWorld()->LineTraceSingleByChannel
	(
		OutHitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);
}

