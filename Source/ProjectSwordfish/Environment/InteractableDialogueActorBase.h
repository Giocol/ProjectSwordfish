#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "InteractableDialogueActorBase.generated.h"

UCLASS()
class PROJECTSWORDFISH_API AInteractableDialogueActorBase : public AActor, public IInteractableInterface{
	GENERATED_BODY()

public:
	AInteractableDialogueActorBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AMainCharacter* InteractionInstigator) override;
};
