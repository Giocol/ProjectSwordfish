#include "MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "ProjectSwordfish/Environment/InteractableInterface.h"
#include "ProjectSwordfish/Environment/SpearableInterface.h"
#include "..\UI\SliderWithTargetAreas.h"
#include "ProjectSwordfish/DataAssets/FishingEventDataAsset.h"


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
	UE_LOG(LogTemp, Error, TEXT("Use shouldn't be called on MainCharacter base class!"))
}

void AMainCharacter::Pull(float DeltaTime) {
	UE_LOG(LogTemp, Error, TEXT("Pull shouldn't be called on MainCharacter base class!"))
}

void AMainCharacter::OnSecondaryAction() {
	UE_LOG(LogTemp, Error, TEXT("OnSecondaryAction shouldn't be called on MainCharacter base class!"))
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

