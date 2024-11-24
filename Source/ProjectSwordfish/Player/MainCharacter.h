// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class UCameraComponent;

UCLASS(Abstract)
class PROJECTSWORDFISH_API AMainCharacter : public ACharacter {
	GENERATED_BODY()

public:
	AMainCharacter();
	
	virtual void ProcessCharacterMovementInput(FVector2D input);
	virtual void ProcessCameraMovementInput(FVector2D Input);
	virtual void ProcessInteract();
	virtual void ProcessUse();

	virtual void Pull(float DeltaTime);
	void SetIsDoingSecondaryAction(bool State) { bIsDoingSecondaryAction = State; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void TraceInteract(FHitResult& OutHitResult) const;

protected:
	UPROPERTY(EditDefaultsOnly)
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
		float InteractionRange = 20.f;

	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		bool bIsDoingSecondaryAction = false;
};
