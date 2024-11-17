#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "SpearInteractable.generated.h"

UCLASS(Abstract)
class PROJECTSWORDFISH_API ASpearInteractable : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ASpearInteractable();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void Interact(AMainCharacter* InteractionInstigator) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* SpearMesh = nullptr;
};
