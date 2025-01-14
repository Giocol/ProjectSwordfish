#include "HearingComponent.h"


UHearingComponent::UHearingComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UHearingComponent::BeginPlay() {
	Super::BeginPlay();

	//ANoiseSystem* NoiseSystemRef = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetNoiseSystemRef();
	//NoiseSystemRef->RegisterListener(this);
//
	//if (AEnemyBase* const OwningAI = Cast<AEnemyBase>(GetOwner()))
	//	OwningAI->RegisterHearing(this);
}

void UHearingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHearingComponent::OnNoiseHeard(const UNoiseDataAsset* NoiseDataAsset, const FVector& Location) {
	////TODO: Add logic for NoiseDataAsset with bSpecifyCustomRadius == false
	//if(NoiseDataAsset->bSpecifyCustomRadius) {
	//	float Distance = FVector::Distance(GetOwner()->GetActorLocation(), Location) / 100.f;
	//	if(Distance < 1.f)
	//		Distance = 1.f;
	//	float newIntensity = NoiseDataAsset->Intensity / FMath::Pow(Distance, NoiseDataAsset->FalloffPower);
	//	DrawDebugSphere(GetWorld(), Location, NoiseDataAsset->Radius, 20, FColor::Purple, false, 1);
	//	if((GetOwner()->GetActorLocation() - Location).Length() < NoiseDataAsset->Radius) {
	//		UE_LOG(LogTemp, Warning, TEXT("Heard noise with intesity %f at location %s"), newIntensity, *Location.ToString());
	//		LastNoiseSignalIntesity = newIntensity;
	//		LastNoiseSignalLocation = Location;
	//		LastNoiseSignal.SignalOrigin = LastNoiseSignalLocation;
	//		LastNoiseSignal.SignalStrength = LastNoiseSignalIntesity;
	//		bHasLastNoiseSignalBeenConsumed = false;
	//	}
	//}
}

FPerceptionSignal UHearingComponent::GetLastNoiseSignal() {
	bHasLastNoiseSignalBeenConsumed = true;
	return LastNoiseSignal;
}

