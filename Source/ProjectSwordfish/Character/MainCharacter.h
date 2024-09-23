// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class PROJECTSWORDFISH_API AMainCharacter : public ACharacter {
	GENERATED_BODY()

public:
	AMainCharacter();
	
	void ProcessCharacterMovementInput(FVector2D input);
	void ProcessCameraMovementInput(FVector2D input) const;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
