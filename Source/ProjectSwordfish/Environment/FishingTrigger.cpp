#include "FishingTrigger.h"

#include "Components/BoxComponent.h"
#include "ProjectSwordfish/Player/MainCharacter.h"


AFishingTrigger::AFishingTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>("Trigger");
	Trigger->SetupAttachment(RootComponent);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AFishingTrigger::OnTriggerBeginOverlap);
}

void AFishingTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFishingTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AMainCharacter* Player = OtherActor ? Cast<AMainCharacter>(OtherActor) : nullptr;
	if(Player && FishingEventDataAsset  && !bHasBeenTriggered) {
		bool bIsFishingStartSuccessful = Player->StartFishingEvent(FishingEventDataAsset);
		if(bIsFishingStartSuccessful)
			bHasBeenTriggered = true; //todo: terrible hack, change
	}
}

void AFishingTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

