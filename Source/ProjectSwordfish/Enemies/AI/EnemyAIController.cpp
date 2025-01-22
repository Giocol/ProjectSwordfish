#include "EnemyAIController.h"

#include "FuzzyBrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemyAIController::AEnemyAIController() {
	Brain = CreateDefaultSubobject<UFuzzyBrainComponent>("Brain");
	
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnSignalOriginChanged(const FVector& NewOrigin) const {
	BlackboardRef->SetValueAsVector("TargetLocation", NewOrigin);
}

void AEnemyAIController::OnInterestChanged(const FWeightedSignal& WeightedSignal) const {
	BlackboardRef->SetValueAsVector("TargetLocation", WeightedSignal.Signal.SignalOrigin);
	BlackboardRef->SetValueAsFloat("TargetSignalWeight", WeightedSignal.Weight);
}

void AEnemyAIController::OnSignalSeverityChanged(const FWeightedSignal& WeightedSignal) const {
	BlackboardRef->SetValueAsFloat("TargetSignalWeight", WeightedSignal.Weight);
}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	if(!BehaviorTreeRef)
		UE_LOG(LogTemp, Error, TEXT("ERROR: Missing BehaviorTreeRef, please plug it in the editor!"));

	BlackboardRef = GetBlackboardComponent();
	RunBehaviorTree(BehaviorTreeRef);
	
	Super::OnPossess(InPawn);
}	
