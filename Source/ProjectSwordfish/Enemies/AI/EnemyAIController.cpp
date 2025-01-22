#include "EnemyAIController.h"

#include "FuzzyBrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"


AEnemyAIController::AEnemyAIController() {
	Brain = CreateDefaultSubobject<UFuzzyBrainComponent>("Brain");
	
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnSignalOriginChanged(FVector NewOrigin) {
}

void AEnemyAIController::OnInterestChanged(FWeightedSignal WeightedSignal) {
}

void AEnemyAIController::OnSignalSeverityChanged(FWeightedSignal WeightedSignal) {
}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	if(!BehaviorTreeRef)
		UE_LOG(LogTemp, Error, TEXT("ERROR: Missing BehaviorTreeRef, please plug it in the editor!"));

	Brain->SetBlackboardRef(GetBlackboardComponent());
	RunBehaviorTree(BehaviorTreeRef);
	
	Super::OnPossess(InPawn);
}	
