#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Spear.generated.h"

UCLASS(Abstract)
class PROJECTSWORDFISH_API ASpear : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ASpear();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void Interact(AMainCharacter* InteractionInstigator) override;
};
