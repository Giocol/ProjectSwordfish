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

	void HandleMovement(const FInputActionValue& value);
	void HandleCameraMovement(const FInputActionValue& value);

private:
	void InitSystems();
	void SetupInputMappingContext();
	void SetupInputActions();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> mappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions | Movement", meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UInputAction> characterMovementAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions | Camera", meta = (AllowPrivateAccess = "true"));
	TObjectPtr<UInputAction> cameraMovementAction = nullptr;

	//refs
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMainCharacter> characterRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnhancedInputComponent> inputComponentRef = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> inputSubsystemRef = nullptr;
};
