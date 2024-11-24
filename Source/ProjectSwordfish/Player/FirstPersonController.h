// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FirstPersonController.generated.h"

class AMainCharacter;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;


UCLASS()
class PROJECTSWORDFISH_API AFirstPersonController : public APlayerController {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void HandleMovement(const FInputActionValue& Value);
	void HandleCameraMovement(const FInputActionValue& Value);
	void HandleInteraction(const FInputActionValue& Value);
	void HandleUse(const FInputActionValue& Value);
	void StartSecondaryAction(const FInputActionValue& Value);
	void EndSecondaryAction(const FInputActionValue& Value);

private:
	void InitSystems();
	void SetupInputMappingContext() const;
	void SetupInputActions();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UInputMappingContext> mappingContext = nullptr;

	//TODO: make this two things handled by settings menu
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Settings", meta = (AllowPrivateAccess = "true"))
		float Sensitivity = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Settings", meta = (AllowPrivateAccess = "true"))
		bool bIsYawInputInverted = false;

	//actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions | Movement", meta = (AllowPrivateAccess = "true"));
		TObjectPtr<UInputAction> CharacterMovementAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions ", meta = (AllowPrivateAccess = "true"));
		TObjectPtr<UInputAction> InteractAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions ", meta = (AllowPrivateAccess = "true"));
		TObjectPtr<UInputAction> UseAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions ", meta = (AllowPrivateAccess = "true"));
		TObjectPtr<UInputAction> PullAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions | Camera", meta = (AllowPrivateAccess = "true"));
		TObjectPtr<UInputAction> CameraMovementAction = nullptr;

	//refs
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<AMainCharacter> characterRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UEnhancedInputComponent> inputComponentRef = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UEnhancedInputLocalPlayerSubsystem> inputSubsystemRef = nullptr;
};
