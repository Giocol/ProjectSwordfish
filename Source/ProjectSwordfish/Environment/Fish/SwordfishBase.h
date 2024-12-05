#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProjectSwordfish/Environment/SpearableInterface.h"
#include "SwordfishBase.generated.h"

UCLASS()
class PROJECTSWORDFISH_API ASwordfishBase : public APawn, public ISpearableInterface {
	GENERATED_BODY()

public:
	ASwordfishBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnSpeared(AMainCharacter* SpearingInstigator) override;

protected:
	UPROPERTY(BlueprintReadOnly)
		bool bIsSpeared = false;
};
