// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class UCameraComponent;

UCLASS()
class PROJECTSWORDFISH_API AMainCharacter : public ACharacter {
	GENERATED_BODY()

public:
	AMainCharacter();
	
	void ProcessCharacterMovementInput(FVector2D input);
	void ProcessCameraMovementInput(FVector2D Input);
	void ProcessInteract();
	void ProcessUse();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void TraceInteract(FHitResult& OutHitResult) const;

protected:
	UPROPERTY(EditDefaultsOnly)
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float InteractionRange = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		UStaticMeshComponent* Spear = nullptr;
};
