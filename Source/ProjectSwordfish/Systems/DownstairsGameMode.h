#pragma once

#include "CoreMinimal.h"
#include "SwordfishGameMode.h"
#include "DownstairsGameMode.generated.h"

class ANoiseSystem;

UCLASS()
class PROJECTSWORDFISH_API ADownstairsGameMode : public ASwordfishGameMode {
	GENERATED_BODY()


public:
	ANoiseSystem* GetNoiseSystemRef() { return  NoiseSystemRef; };

	void InitGameState() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Systems", meta = (AllowPrivateAccess = true))
		TSubclassOf<ANoiseSystem> NoiseSystemClass = nullptr;

	UPROPERTY()
		TObjectPtr<ANoiseSystem> NoiseSystemRef = nullptr;
};
