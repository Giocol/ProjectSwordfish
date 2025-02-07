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
	void OnSignalOriginChanged(const FVector& NewOrigin) const;
	void OnInterestChanged(const FWeightedSignal& WeightedSignal) const;
	void OnSignalSeverityChanged(const FWeightedSignal& WeightedSignal) const;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBehaviorTree* BehaviorTreeRef = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UFuzzyBrainComponent* Brain = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AI")
		UBlackboardComponent* BlackboardRef = nullptr;
};
