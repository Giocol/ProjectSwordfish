#include "EnemyAIController.h"

#include "FuzzyBrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemyAIController::AEnemyAIController() {
	Brain = CreateDefaultSubobject<UFuzzyBrainComponent>("Brain");
	
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnSignalOriginChanged(const FVector& NewOrigin) const {
	if(BlackboardRef)
		BlackboardRef->SetValueAsVector("TargetLocation", NewOrigin);
	else
		UE_LOG(LogTemp, Error, TEXT("MISSING BLACKBOARD REF IN AICONTROLLER"))
}

void AEnemyAIController::OnInterestChanged(const FWeightedSignal& WeightedSignal) const {
	if(BlackboardRef) {
		BlackboardRef->SetValueAsVector("TargetLocation", WeightedSignal.Signal.SignalOrigin);
		BlackboardRef->SetValueAsFloat("TargetSignalWeight", WeightedSignal.Weight);
	} else
		UE_LOG(LogTemp, Error, TEXT("MISSING BLACKBOARD REF IN AICONTROLLER"))
}

void AEnemyAIController::OnSignalSeverityChanged(const FWeightedSignal& WeightedSignal) const {
	if(BlackboardRef)
		BlackboardRef->SetValueAsFloat("TargetSignalWeight", WeightedSignal.Weight);
	else
		UE_LOG(LogTemp, Error, TEXT("MISSING BLACKBOARD REF IN AICONTROLLER"))
}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	if(!BehaviorTreeRef)
		UE_LOG(LogTemp, Error, TEXT("ERROR: Missing BehaviorTreeRef, please plug it in the editor!"));

	BlackboardRef = GetBlackboardComponent();
	RunBehaviorTree(BehaviorTreeRef);
	
	Super::OnPossess(InPawn);
}	
