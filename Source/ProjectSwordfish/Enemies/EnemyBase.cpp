#include "EnemyBase.h"

#include "AI/PerceptionSignal.h"


AEnemyBase::AEnemyBase() {
	PrimaryActorTick.bCanEverTick = true;
}

bool AEnemyBase::IsActorInView(AEnemyBase* Target, AActor* Actor, float& SignalStrength) {
	return false;
}

bool AEnemyBase::IsLocationInView(AEnemyBase* Target, FVector Location, float Tolerance, float& SignalStrength) {
	return false;
}

FPerceptionSignal AEnemyBase::GetVisionSignalToPlayer(AEnemyBase* Target) {
	return FPerceptionSignal();
}

TArray<FPerceptionSignal> AEnemyBase::GetVisionSignalsOfClass(AEnemyBase* Target, TSubclassOf<AActor> Class) {
	return TArray<FPerceptionSignal>();
}

TArray<FPerceptionSignal> AEnemyBase::GetVisionSignals(AEnemyBase* Target) {
	return TArray<FPerceptionSignal>();
}

FPerceptionSignal AEnemyBase::GetLastHearingSignal(AEnemyBase* Target) {
	return FPerceptionSignal();
}

bool AEnemyBase::HasNewSignalBeenHeard(AEnemyBase* Target) {
	return false;
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
	return TArray<AActor*>();
}

TArray<AActor*> AEnemyBase::GetVisibleActorCandidates() const {
	return TArray<AActor*>();
}

void AEnemyBase::BeginPlay() {
	Super::BeginPlay();
	
}

void AEnemyBase::UpdateNavigationArrays() const {
}

void AEnemyBase::DeleteAllWaypoints() const {
}

void AEnemyBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

