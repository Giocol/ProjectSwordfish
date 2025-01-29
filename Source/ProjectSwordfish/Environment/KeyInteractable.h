// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "KeyInteractable.generated.h"

UCLASS()
class PROJECTSWORDFISH_API AKeyInteractable : public AActor, public IInteractableInterface {
	GENERATED_BODY()

public:
	virtual void Interact(AMainCharacter* InteractionInstigator) override;

protected:
	UPROPERTY(BlueprintReadOnly)
		AMainCharacter* PickupInstigator;
};
