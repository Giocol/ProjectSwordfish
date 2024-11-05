#include "MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "ProjectSwordfish/Environment/InteractableInterface.h"
#include "ProjectSwordfish/Environment/SpearableInterface.h"


AMainCharacter::AMainCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(RootComponent);
	Spear = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spear Mesh"));
	Spear->SetupAttachment(RootComponent);
	Spear->OnComponentHit.AddDynamic(this, &AMainCharacter::OnSpearHit);
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
	if(bHasSpear)
	{
		UE_LOG(LogTemp, Warning, TEXT("I'm throwing the spear!"));

		//TODO: we're not gonna use physics later, it was just to get it done quick and dirty
		Spear->SetSimulatePhysics(true);
		Spear->AddImpulse(Spear->GetForwardVector()*SpearSpeed);
	}
}

void AMainCharacter::OnSpearHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit) {

	if(OtherActor->Implements<USpearableInterface>()) {
		//NOTE: this only works for C++, if the interface is implemented directly in blueprint weird shit happens, look into it!
		ISpearableInterface* OverlappedSpearable = Cast<ISpearableInterface>(OtherActor);
		OverlappedSpearable->OnSpeared(this);
		CurrentlySpearedActor = OtherActor;
	}
}

void AMainCharacter::BeginPlay() {
	Spear->SetVisibility(false);
	
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

