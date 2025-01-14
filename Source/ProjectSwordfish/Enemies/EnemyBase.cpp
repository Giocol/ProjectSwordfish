#include "EnemyBase.h"

#include "AI/HearingComponent.h"
#include "AI/PerceptionSignal.h"
#include "AI/SightComponent.h"
#include "Kismet/GameplayStatics.h"


AEnemyBase::AEnemyBase() {
	PrimaryActorTick.bCanEverTick = true;
	
	//IdleWaypointHolder = CreateDefaultSubobject<UWaypointHolderComponent>("Idle Waypoint Holder");
	//SuspiciousWaypointHolder = CreateDefaultSubobject<UWaypointHolderComponent>("Alert Waypoint Holder");
}

bool AEnemyBase::IsActorInView(AEnemyBase* Target, AActor* Actor, float& SignalStrength) {
	SignalStrength = 0.f;
	for(auto SightComponent : Target->SightComponents) {
		float currentComponentDetectionRate;
		if(SightComponent->IsActorVisible(Actor, currentComponentDetectionRate)) {
			if(currentComponentDetectionRate > SignalStrength)
				SignalStrength = currentComponentDetectionRate;
		}
	}
	return SignalStrength > 0;
}

bool AEnemyBase::IsLocationInView(AEnemyBase* Target, FVector Location, float Tolerance, float& SignalStrength) {
	SignalStrength = 0.f;
	for(auto SightComponent : Target->SightComponents) {
		float currentComponentDetectionRate;
		if(SightComponent->IsLocationVisible(Location, Tolerance, currentComponentDetectionRate)) {
			if(currentComponentDetectionRate > SignalStrength)
				SignalStrength = currentComponentDetectionRate;
		}
	}
	return SignalStrength > 0;
}

FPerceptionSignal AEnemyBase::GetVisionSignalToPlayer(AEnemyBase* Target) {
	AActor* Player = UGameplayStatics::GetPlayerController(Target->GetWorld(), 0)->GetPawn();
	float SignalStrength;
	if(IsActorInView(Target, Player, SignalStrength))
		return FPerceptionSignal(SignalStrength, Player);
	else return FPerceptionSignal();
}

TArray<FPerceptionSignal> AEnemyBase::GetVisionSignalsOfClass(AEnemyBase* Target, TSubclassOf<AActor> Class) {
	TArray<FPerceptionSignal> Result;
	for(auto Actor : Target->GetVisibleActorCandidatesOfClass(Class)) {
		float DetectionRate;
		if(IsActorInView(Target, Actor, DetectionRate))
			Result.Add(FPerceptionSignal(DetectionRate, Actor));
	}
	return Result;
}

TArray<FPerceptionSignal> AEnemyBase::GetVisionSignals(AEnemyBase* Target) {
	return GetVisionSignalsOfClass(Target, AActor::StaticClass());
}

FPerceptionSignal AEnemyBase::GetLastHearingSignal(AEnemyBase* Target) {
	return  Target->GetHearingComponent()->GetLastNoiseSignal();
}

bool AEnemyBase::HasNewSignalBeenHeard(AEnemyBase* Target) {
	return Target && Target->GetHearingComponent() && Target->GetHearingComponent()->HasNewSignalBeenHeard();
}

void AEnemyBase::SaveState() {
}

void AEnemyBase::LoadState() {
}

UFuzzyBrainComponent* AEnemyBase::GetBrain() const {
	return nullptr;
}

void AEnemyBase::Die(const AActor* Killer) {
}

TArray<AActor*> AEnemyBase::GetVisibleActorCandidatesOfClass(TSubclassOf<AActor> Class) const {
	TArray<AActor*> Candidates;
	for (auto SightComponent : SightComponents) {
		for (auto Candidate : SightComponent->GetActorsInVisionCone()) {
			if(Candidate->IsA(Class))
				Candidates.AddUnique(Candidate);
		}
	}
	return Candidates;
}

TArray<AActor*> AEnemyBase::GetVisibleActorCandidates() const {
	return GetVisibleActorCandidatesOfClass(AActor::StaticClass());
}

void AEnemyBase::BeginPlay() {
	Super::BeginPlay();
	
}

void AEnemyBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


#if WITH_EDITOR
void AEnemyBase::UpdateNavigationArrays() const {
	//IdleWaypointHolder->UpdateWaypointArray(NumberOfIdleWaypoints, "Idle");
	//SuspiciousWaypointHolder->UpdateWaypointArray(NumberOfSuspiciousWaypoints, "Alert");
}

void AEnemyBase::DeleteAllWaypoints() const {
	//IdleWaypointHolder->DeleteAllWaypoints();
	//SuspiciousWaypointHolder->DeleteAllWaypoints();
}
#endif
