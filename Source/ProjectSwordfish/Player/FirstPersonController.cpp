#include "FirstPersonController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainCharacter.h"

void AFirstPersonController::BeginPlay() {
	Super::BeginPlay();
}

void AFirstPersonController::OnPossess(APawn* InPawn) {
	characterRef = Cast<AMainCharacter>(InPawn);
	checkf(characterRef, TEXT("Missing AMainCharacter OnPosses"));

	
	inputSubsystemRef = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	checkf(inputSubsystemRef, TEXT("missing UEnhancedInputLocalPlayerSubsystem OnPosses"))

	inputComponentRef = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(inputSubsystemRef, TEXT("missing UEnhancedInputComponent OnPosses"))

	SetupInputMappingContext();
	SetupInputActions();
	Super::OnPossess(InPawn);
}

void AFirstPersonController::OnUnPossess() {
	Super::OnUnPossess();
}

void AFirstPersonController::HandleMovement(const FInputActionValue& Value) {
	const FVector2D Axis = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("Movement input: %f, %f"), Axis.X, Axis.Y);
	characterRef->ProcessCharacterMovementInput(Axis);
}

void AFirstPersonController::HandleCameraMovement(const FInputActionValue& Value) {
	FVector2D Axis = Value.Get<FVector2D>() * Sensitivity;
	if(!bIsYawInputInverted)
		Axis.X = -Axis.X;
	//UE_LOG(LogTemp, Warning, TEXT("Camera input: %f, %f"), Axis.X, Axis.Y);
	characterRef->ProcessCameraMovementInput(Axis);
}

void AFirstPersonController::HandleInteraction(const FInputActionValue& Value) {
	characterRef->ProcessInteract();
}

void AFirstPersonController::HandleUse(const FInputActionValue& Value) {
	characterRef->ProcessUse();
}

void AFirstPersonController::StartSecondaryAction(const FInputActionValue& Value) {
	characterRef->SetIsDoingSecondaryAction(true);
}

void AFirstPersonController::EndSecondaryAction(const FInputActionValue& Value) {
	characterRef->SetIsDoingSecondaryAction(false);
}

void AFirstPersonController::SetupInputMappingContext() const
{
	inputSubsystemRef->ClearAllMappings();
	inputSubsystemRef->AddMappingContext(mappingContext, 0);
}

void AFirstPersonController::SetupInputActions() {
	if(!CharacterMovementAction)
		UE_LOG(LogTemp, Error, TEXT("Missing character movement action ref! Please plug it in the FirstPersonController BP!"));
	
	if(!CameraMovementAction)
		UE_LOG(LogTemp, Error, TEXT("Missing camera movement action ref! Please plug it in the FirstPersonController BP!"));

	if(!InteractAction)
		UE_LOG(LogTemp, Error, TEXT("Missing interact action ref! Please plug it in the FirstPersonController BP!"));

	if(!UseAction)
		UE_LOG(LogTemp, Error, TEXT("Missing use action ref! Please plug it in the FirstPersonController BP!"));
	
	inputComponentRef->BindAction(CharacterMovementAction, ETriggerEvent::Triggered, this, &AFirstPersonController::HandleMovement);
	inputComponentRef->BindAction(CameraMovementAction, ETriggerEvent::Triggered, this, &AFirstPersonController::HandleCameraMovement);
	inputComponentRef->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AFirstPersonController::HandleInteraction);
	inputComponentRef->BindAction(UseAction, ETriggerEvent::Triggered, this, &AFirstPersonController::HandleUse);
	inputComponentRef->BindAction(PullAction, ETriggerEvent::Ongoing, this, &AFirstPersonController::StartSecondaryAction);
	inputComponentRef->BindAction(PullAction, ETriggerEvent::Triggered, this, &AFirstPersonController::EndSecondaryAction);
}
