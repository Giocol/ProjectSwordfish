#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "EnemyAIController.generated.h"

struct FWeightedSignal;
class UFuzzyBrainComponent;

UCLASS()
class PROJECTSWORDFISH_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	void OnSignalOriginChanged(FVector NewOrigin);
	void OnInterestChanged(FWeightedSignal WeightedSignal);
	void OnSignalSeverityChanged(FWeightedSignal WeightedSignal);
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBehaviorTree* BehaviorTreeRef = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UFuzzyBrainComponent* Brain = nullptr;
};
