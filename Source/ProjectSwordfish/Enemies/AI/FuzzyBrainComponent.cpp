#include "FuzzyBrainComponent.h"

#include "EnemyAIController.h"
#include "ProjectSwordfish/Enemies/EnemyBase.h"
#include "ProjectSwordfish/Enemies/EnemyState.h"


UFuzzyBrainComponent::UFuzzyBrainComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UFuzzyBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(!bIsThinking)
		return;

	uint32 NewHighestId = GetSignalIdOfHighestWeight();
	if(NewHighestId != INDEX_NONE
		&& ((HighestWeightId == INDEX_NONE || HighestWeightId > static_cast<uint32>(Memory.Num()))
		|| Memory[NewHighestId].Signal != Memory[HighestWeightId].Signal)) {
		HighestWeightId = NewHighestId;
		if(auto Owner = Cast<AEnemyAIController>(GetOwner()))
			Owner->OnInterestChanged(Memory[NewHighestId]);
			UpdateEnemyState();
		}

	See(DeltaTime); 
	Hear(DeltaTime);
	ForgetUnimportant();
	
	for(auto& WeightedSignal : Memory) {
		UpdateSignal(WeightedSignal, DeltaTime);
		if(!WeightedSignal.bPositiveSlopeSign) {
			Decrement(WeightedSignal, DeltaTime);
		}
		WeightedSignal.bPositiveSlopeSign = false;
	}

	if(HighestWeightId < static_cast<uint32>(Memory.Num())) {
		if(GetSeverity(Memory[HighestWeightId]) != LastRecordedSeverity) {
			if(Controller && Body && NewHighestId != INDEX_NONE && static_cast<int>(NewHighestId) < Memory.Num()) {
				Controller->OnSignalSeverityChanged(Memory[NewHighestId]);
				//Body->GetEnemyManager()->RegisterSeverityChange(GetSeverity(Memory[HighestWeightId]));
			}
			UpdateEnemyState(); //TODO: temp just for testing
		}
	}

	for (uint64 i = 0; i < Memory.Num(); i++) {
		GEngine->AddOnScreenDebugMessage(i+GetUniqueID(), DeltaTime, FColor::Emerald,
			FString::Printf(TEXT("%f"), Memory[i].GetWeight()));
	}

	if(HighestWeightId < static_cast<uint32>(Memory.Num()))
		LastRecordedSeverity = GetSeverity(Memory[HighestWeightId]);
}

void UFuzzyBrainComponent::BeginPlay() {
	Super::BeginPlay();
	Controller = Cast<AEnemyAIController>(GetOwner());
	if(Controller)
		Body = Cast<AEnemyBase>(Controller->GetPawn());
}

ESignalSeverity UFuzzyBrainComponent::GetSeverity(FWeightedSignal WeightedSignal) const noexcept {
	return GetSeverityFromWeight(WeightedSignal.Weight);
	
}

ESignalSeverity UFuzzyBrainComponent::GetSeverityFromWeight(float Weight) const noexcept {
	if(Weight < SignalWeightThresholds.WeakSignalThreshold)
		return ESignalSeverity::Nonperceptible;
	else if(Weight < SignalWeightThresholds.MediumSignalThreshold)
		return ESignalSeverity::Weak;
	else if(Weight < SignalWeightThresholds.StrongSignalThreshold)
		return ESignalSeverity::Medium;
	else
		return ESignalSeverity::Strong;
}


void UFuzzyBrainComponent::GetSeverity_Branching(FWeightedSignal WeightedSignal,
                                                 ESignalSeverity& Branches) {
	Branches = GetSeverity(WeightedSignal);
}

bool UFuzzyBrainComponent::IsValid(FWeightedSignal WeightedSignal) const {
	return HasMemory(WeightedSignal.Signal);
}

void UFuzzyBrainComponent::IsValid_Branching(FWeightedSignal WeightedSignal, bool& Result) {
	Result = IsValid(WeightedSignal);
}


void UFuzzyBrainComponent::RegisterSignalToMemory(double DeltaTime, FPerceptionSignal Signal) {
	if(Signal.Actor) {
		if(!ClassPrejudice.ContainsByPredicate([Signal](FWeightedClass c)
			{ return Signal.Actor->IsA(c.Class); }))
			return;
		for(int i = 0; i < Memory.Num(); i++) {
			if(Memory[i].Signal.Actor == Signal.Actor) {
				IncrementCompoundingWeight(Memory[i], DeltaTime);
				Memory[i].Signal.SignalStrength = Signal.SignalStrength;
				if(i == HighestWeightId)
					return;
			}
		}
	}
	Memory.Add(Signal);
}

bool UFuzzyBrainComponent::TryResolvePointOfInterest(FPerceptionSignal Signal) {
	FWeightedSignal InArray;
	if(IsResolvable(Signal, InArray)) {
		Memory.Remove(InArray);
		if(static_cast<int>(HighestWeightId) < Memory.Num() && HighestWeightId != INDEX_NONE && Memory[HighestWeightId].Signal == Signal)
			LastRecordedSeverity = ESignalSeverity::Nonperceptible;
		HighestWeightId = INDEX_NONE; //TODO: temp
		return true;
	} else {
		AEnemyAIController* AiController = Cast<AEnemyAIController>(GetOwner());
		AiController->OnInterestChanged(Signal);
		return false;
	}
}

bool UFuzzyBrainComponent::IsResolvable(FPerceptionSignal Signal, FWeightedSignal& InMemory) const {
	auto InArray = Memory.FindByPredicate([Signal](FWeightedSignal s)
		{ return s.Signal == Signal; });
	if (!InArray || !InArray->IsResolvable())
		return false;
	InMemory = *InArray;
	return true;
}

float UFuzzyBrainComponent::GetNormalizedWeight(AActor* Actor) const {
	if(auto inMemory = Memory.FindByPredicate([Actor](FWeightedSignal WeightedSignal) {
			return WeightedSignal.Signal.Actor == Actor;
		})) {
		float Result = inMemory->GetWeight() / MaxInterest;
		return Result < 1.f ? Result : 1.f;
	}
	return 0;
}

void UFuzzyBrainComponent::Reset() {
	Memory.Empty();
	HighestWeightId = INDEX_NONE;
	LastRecordedSeverity = ESignalSeverity::Nonperceptible;
	bIsThinking = true;
}

void UFuzzyBrainComponent::See(double DeltaTime) {
	if(!Controller)
		return;
	if(Body){
		auto Signals = AEnemyBase::GetVisionSignals(Body);
		for (auto& Signal : Signals) {
			RegisterSignalToMemory(DeltaTime, Signal);
		}
	}
}

void UFuzzyBrainComponent::Hear(double DeltaTime) {
	if(Body) {
		if (AEnemyBase::HasNewSignalBeenHeard(Body) ) {
			RegisterSignalToMemory(DeltaTime, AEnemyBase::GetLastHearingSignal(Body));
		}
	}
}

void UFuzzyBrainComponent::UpdateSignal(FWeightedSignal& WeightedSignal, double DeltaTime) {
	auto Actor = WeightedSignal.Signal.Actor;
	if(Actor && WeightedSignal.GetWeight() > SignalWeightThresholds.StrongSignalThreshold) {
		WeightedSignal.Signal.SignalOrigin = Actor->GetActorLocation();

		AEnemyAIController* AiController = Cast<AEnemyAIController>(GetOwner());
		if(Controller)  // BUG THIS HAPPENS FOR ALL SIGNALS
			AiController->OnSignalOriginChanged(WeightedSignal.Signal.SignalOrigin);
	}
}

void UFuzzyBrainComponent::ForgetUnimportant() {
	int Num = Memory.Num();
	for(int i = 0; i < Num; i++) {
		if(Memory[i].GetWeight() < ForgetThreshold && !Memory[i].bPositiveSlopeSign) {
			Memory.RemoveAt(i);
			if(HighestWeightId >= static_cast<uint32>(i))
				HighestWeightId--;
			i--;
			Num--;
		}
	}
}

uint32 UFuzzyBrainComponent::GetSignalIdOfHighestWeight() {
	uint32 Result = 0;
	if(Memory.IsEmpty())
		return INDEX_NONE;
	for (int Id = 0; Id < Memory.Num(); Id++) {
		if(static_cast<int>(Result) < Memory.Num() && Memory[Id].Weight > Memory[Result].Weight)
			Result = Id;
	}
	return Result;
}

void UFuzzyBrainComponent::UpdateEnemyState() const {
	//TODO:this sucks, do it in the controller instead?
	AEnemyBase* const EnemyPawn = Cast<AEnemyBase>(Cast<AEnemyAIController>(GetOwner())->GetPawn());
	if(!EnemyPawn)
		return;

	const ESignalSeverity Severity = GetSeverity(Memory[HighestWeightId]);
	
	if(Severity == ESignalSeverity::Medium && EnemyPawn->GetCurrentState() == EEnemyState::Idle)
		EnemyPawn->SetCurrentState(EEnemyState::Suspicious);
	else if(Severity == ESignalSeverity::Strong && EnemyPawn->GetCurrentState() == EEnemyState::Suspicious)
		EnemyPawn->SetCurrentState(EEnemyState::Agitated);
}

void UFuzzyBrainComponent::IncrementCompoundingWeight(FWeightedSignal& WeightedSignal, double DeltaTime) {
	auto Actor = WeightedSignal.Signal.Actor;
	if(Actor && !ClassPrejudice.IsEmpty()) {
		auto ActorPrejudice = ClassPrejudice.FindByPredicate([this, Actor](FWeightedClass WeightedClass)
		{ return Actor->IsA(WeightedClass.Class); });
		float Weight = DefaultPrejudice;
		if(ActorPrejudice) {
			Weight = ActorPrejudice->Weight;
		}
		float Increment = Weight * DeltaTime;
		auto a = WeightedSignal.Signal.Actor;
		
		WeightedSignal.Weight += Increment * WeightedSignal.Signal.SignalStrength;
		WeightedSignal.bPositiveSlopeSign = true;
		if(WeightedSignal.Weight > MaxInterest) 
			WeightedSignal.Weight = MaxInterest;
	}
}

void UFuzzyBrainComponent::Decrement(FWeightedSignal& WeightedSignal, double DeltaTime) {
	ESignalSeverity PreviousSeverity = GetSeverity(WeightedSignal);
	if(WeightedSignal.Weight > MaxInterest)
		WeightedSignal.Weight = MaxInterest;
	WeightedSignal.Weight *= FMath::Exp(-DeltaTime * PrejudiceDecay);
	if(HighestWeightId != INDEX_NONE && HighestWeightId < Memory.Num() && WeightedSignal.Signal == Memory[HighestWeightId]
		&& PreviousSeverity != GetSeverity(WeightedSignal)) {
		
	}
		
}


bool UFuzzyBrainComponent::HasMemory(FPerceptionSignal Signal) const {
	for(auto WeightedSignal : Memory) {
		if(WeightedSignal.Signal == Signal)
			return true;
	}
	return false;
}

