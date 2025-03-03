﻿#include "FishingTrigger.h"

#include "Components/BoxComponent.h"
#include "ProjectSwordfish/DataAssets/FishingEventDataAsset.h"
#include "ProjectSwordfish/Player/MainCharacter.h"
#include "ProjectSwordfish/Player/UpstairsCharacter.h"


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

	if(!Swordfish) {
		UE_LOG(LogTemp, Error, TEXT("MISSING SWORDFISH REFERENCE, please plug it into the inspector"))
		return;
	}

	FishingEventDataAsset->Swordfish = Swordfish;
}

void AFishingTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AUpstairsCharacter* Player = OtherActor ? Cast<AUpstairsCharacter>(OtherActor) : nullptr;
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

