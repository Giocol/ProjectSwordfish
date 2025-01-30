#include "HearingComponent.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectSwordfish/DataAssets/NoiseDataAsset.h"
#include "ProjectSwordfish/Enemies/EnemyBase.h"
#include "ProjectSwordfish/Systems/DownstairsGameMode.h"
#include "ProjectSwordfish/Systems/NoiseSystem.h"


UHearingComponent::UHearingComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UHearingComponent::BeginPlay() {
	Super::BeginPlay();

	//ANoiseSystem* NoiseSystemRef = Cast<ADownstairsGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetNoiseSystemRef();
	//NoiseSystemRef->RegisterListener(this);

	if (AEnemyBase* const OwningAI = Cast<AEnemyBase>(GetOwner()))
		OwningAI->RegisterHearing(this);
}

void UHearingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHearingComponent::OnNoiseHeard(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location) {
	float Radius = NoiseDataAsset->bSpecifyCustomRadius ? NoiseDataAsset->Radius : NoiseDataAsset->Intensity * RadiusMultiplier;
	
	float Distance = FVector::Distance(GetOwner()->GetActorLocation(), Location) / 100.f;
	if(Distance < 1.f)
		Distance = 1.f;
	float NewIntensity = NoiseDataAsset->Intensity / FMath::Pow(Distance, NoiseDataAsset->FalloffPower);
	DrawDebugSphere(GetWorld(), Location, Radius, 20, FColor::Purple, false, 1);
	if((GetOwner()->GetActorLocation() - Location).Length() < Radius) {
		UE_LOG(LogTemp, Warning, TEXT("Heard noise with intesity %f at location %s"), NewIntensity, *Location.ToString());
		LastNoiseSignalIntesity = NewIntensity;
		LastNoiseSignalLocation = Location;
		LastNoiseSignal.SignalOrigin = LastNoiseSignalLocation;
		LastNoiseSignal.SignalStrength = LastNoiseSignalIntesity;
		bHasLastNoiseSignalBeenConsumed = false;
	}
}

FPerceptionSignal UHearingComponent::GetLastNoiseSignal() {
	bHasLastNoiseSignalBeenConsumed = true;
	return LastNoiseSignal;
}

