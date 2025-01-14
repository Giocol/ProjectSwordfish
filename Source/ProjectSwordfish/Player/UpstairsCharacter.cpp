#include "UpstairsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/FishingQTEHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnitConversion.h"
#include "ProjectSwordfish/DataAssets/FishingEventDataAsset.h"
#include "ProjectSwordfish/Environment/SpearableInterface.h"
#include "ProjectSwordfish/Environment/Fish/SwordfishBase.h"
#include "ProjectSwordfish/Utils/MathUtils.h"


AUpstairsCharacter::AUpstairsCharacter() {
	PrimaryActorTick.bCanEverTick = true;
	
	Spear = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spear Mesh"));
	Spear->SetupAttachment(Camera);
	Spear->OnComponentHit.AddDynamic(this, &AUpstairsCharacter::OnSpearHit);
	PullTarget = CreateDefaultSubobject<USceneComponent>(TEXT("Pull Target"));
	PullTarget->SetupAttachment(RootComponent);
	QTEHandler = CreateDefaultSubobject<UFishingQTEHandler>(TEXT("QTE Handler"));
}

void AUpstairsCharacter::BeginPlay() {
	Super::BeginPlay();
	SpearOriginalRotation = Spear->GetComponentRotation();
	Spear->SetVisibility(false);
}

bool AUpstairsCharacter::StartFishingEvent(UFishingEventDataAsset* FishingEventData) {
	if(!bHasSpear)
		return false;
	
	bIsFishing = true;
	CurrentFishingEvent = FishingEventData;

	OnFishingStarted();
	return true;
}

FFishingSliderData AUpstairsCharacter::GetFishingSliderData(EFishingSliderType Type) const {
	if(!CurrentFishingEvent)
		return FFishingSliderData();
	
	if(Type == EFishingSliderType::Aiming) {
		return FFishingSliderData(CurrentAim,
			FVector2d(CurrentFishingEvent->AimLowerThreshold, CurrentFishingEvent->AimUpperThreshold));
	} else if(Type == EFishingSliderType::Power) {
		return FFishingSliderData(CurrentPower,
			FVector2d(CurrentFishingEvent->PowerGoodLowerThreshold, CurrentFishingEvent->PowerGoodUpperThreshold),
			FVector2d(CurrentFishingEvent->PowerMediumLowerThreshold, CurrentFishingEvent->PowerMediumUpperThreshold));
	} else { //Default case, should never occur
		UE_LOG(LogTemp, Error, TEXT("EFishingSliderType passed was None! Something went wrong!"));
		return FFishingSliderData();
	}
}

float AUpstairsCharacter::GetFishingSliderValue(EFishingSliderType Type) const {
	if(Type == EFishingSliderType::Aiming) {
		return CurrentAim;
	} else if(Type == EFishingSliderType::Power) {
		return CurrentPower;
	} else { //Default case, should never occur
		UE_LOG(LogTemp, Error, TEXT("EFishingSliderType passed was None! Something went wrong!"));
		return -1;
	}
}

void AUpstairsCharacter::ProcessCharacterMovementInput(FVector2D input) {
	if(!bIsFishing)
		Super::ProcessCharacterMovementInput(input);
}

void AUpstairsCharacter::ProcessCameraMovementInput(FVector2D Input) {
	if(!bIsFishing)
		Super::ProcessCameraMovementInput(Input);
	else
		CurrentAim = FMath::Clamp((CurrentAim + FMath::Sign(Input.Y) * AimStep * UGameplayStatics::GetWorldDeltaSeconds(GetWorld())), 0.f, 1.f);
}

void AUpstairsCharacter::Pull(float DeltaTime) {
	//todo: this is a terrible name for the function
	if(bIsFishing) {
		CurrentPower = FMath::Clamp(CurrentPower + PowerStep * DeltaTime, 0.0f, 1.f);
	}
	
	//todo: this is all temp
	if(!CurrentlySpearedActor)
		return;
	//todo: don't move on z

	PulledTime += DeltaTime;
	CurrentlySpearedActor->SetActorLocation(
		FMath::Lerp(
				SpearedActorOriginalLocation,
				PullTarget->GetComponentLocation(),
				GetPullProgress()
			));

	//todo: maybe detect overlap with a SuccessfulPullBox or smth rather than this hacky math
	if((CurrentlySpearedActor->GetActorLocation() - PullTarget->GetComponentLocation()).Length() < 10.f) {
		if(!CurrentlySpearedActor->Implements<USpearableInterface>())
			return;
		//NOTE: this only works for C++, if the interface is implemented directly in blueprint weird shit happens, look into it!
		ISpearableInterface* OverlappedSpearable = Cast<ISpearableInterface>(CurrentlySpearedActor);
		OverlappedSpearable->OnPullCompleted(this);
		CurrentlySpearedActor = nullptr;
		bIsFishing = false;
		OnFishingEnded();
		//TODO: SAVE THE RESULT OF THE FISHING! In the dialogue state maybe?
	}
}

void AUpstairsCharacter::ProcessUse() {
	Super::ProcessUse();
	if(bHasSpear && bIsFishing) {
		if(bIsAimInThreshold && bIsPowerInGoodThreshold) {
			// todo: expand this condition, add timer before being able to throw
			UE_LOG(LogTemp, Warning, TEXT("YOU AIMED RIGHT!!!!"));

			//TODO: we're not gonna use physics later, it was just to get it done quick and dirty
			Spear->SetSimulatePhysics(true);
			Spear->AddImpulse(Spear->GetForwardVector()*SpearSpeed);
		}
	}
}

void AUpstairsCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(bIsFishing)
		FishingTick(DeltaTime);
	
	if(bIsDoingSecondaryAction && bIsFishing)
		Pull(DeltaTime);
}

void AUpstairsCharacter::FishingTick(float DeltaTime) {
	if(CurrentFishingEvent->AimLowerThreshold < CurrentAim && CurrentAim < CurrentFishingEvent->AimUpperThreshold) {
		bIsAimInThreshold = true;
	}
	else {
		bIsAimInThreshold = false;
	}

	if(CurrentFishingEvent->PowerGoodLowerThreshold < CurrentPower && CurrentPower < CurrentFishingEvent->PowerGoodUpperThreshold) {
		bIsPowerInGoodThreshold = true;
	}
	else {
		bIsPowerInGoodThreshold = false;
	}

	//todo: medium power should allow you to still throw at a consenquence (hurt yourself and have it reflected at night?)
	if(CurrentFishingEvent->PowerMediumLowerThreshold < CurrentPower && CurrentPower < CurrentFishingEvent->PowerMediumUpperThreshold) {
		bIsPowerInMediumThreshold = true;
	}
	else {
		bIsPowerInMediumThreshold = false;
	}

	AdjustAimVisuals(DeltaTime);

	ApplyFishingResistance(DeltaTime);

	CurrentPower = FMath::Clamp(CurrentPower - PowerDecayPerTick * DeltaTime, 0.f, 1.f);
}

void AUpstairsCharacter::AdjustAimVisuals(float DeltaTime) {
	if(CurrentlySpearedActor)
		return;
	
	FRotator CorrectLookAtRot = (-Spear->GetComponentLocation() + CurrentFishingEvent->Swordfish->GetActorLocation()).Rotation();
	float CurrentLookAtPitch = FMathUtils::DegAngleLerp(SpearOriginalRotation.Pitch, CorrectLookAtRot.Pitch, GetPowerProgress());
	float CurrentLookAtYaw = FMathUtils::DegAngleLerp(-SpearOriginalRotation.Yaw, CorrectLookAtRot.Yaw, GetAimingProgress());
	UE_LOG(LogTemp, Error, TEXT("%f"), CurrentLookAtYaw);
	Spear->SetWorldRotation(FRotator(CurrentLookAtPitch, CurrentLookAtYaw, CorrectLookAtRot.Roll));
}

void AUpstairsCharacter::ApplyFishingResistance(float DeltaTime) {
	if(CurrentAim < CurrentFishingEvent-> AimLowerThreshold + (CurrentFishingEvent->AimUpperThreshold - CurrentFishingEvent->AimLowerThreshold)/2)
		CurrentAim = FMath::Clamp(CurrentAim - AimResistancePerTick * DeltaTime, 0.f, 1.f);
	else
		CurrentAim = FMath::Clamp(CurrentAim + AimResistancePerTick * DeltaTime, 0.f, 1.f);
}

float AUpstairsCharacter::GetPullProgress() const {
	return PulledTime / TimeToPull;
}

float AUpstairsCharacter::GetAimingProgress() const {
	if(bIsAimInThreshold)
		return 1;
	if(CurrentAim < CurrentFishingEvent->AimLowerThreshold)
		return CurrentAim / CurrentFishingEvent->AimLowerThreshold;
	else {
		return  1 - ((CurrentAim - CurrentFishingEvent->AimUpperThreshold) / (1 - CurrentFishingEvent->AimUpperThreshold));
	}
}


float AUpstairsCharacter::GetPowerProgress() const {
	if(bIsPowerInGoodThreshold || bIsPowerInMediumThreshold)
		return 1;
	if(CurrentPower < CurrentFishingEvent->PowerMediumLowerThreshold)
		return CurrentPower / CurrentFishingEvent->PowerMediumLowerThreshold;
	else {
		return  1 - ((CurrentPower - CurrentFishingEvent->PowerMediumUpperThreshold) / (1 - CurrentFishingEvent->PowerMediumUpperThreshold));
	}
}

void AUpstairsCharacter::OnSpearHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                    FVector NormalImpulse, const FHitResult& Hit) {

	if(OtherActor->Implements<USpearableInterface>()) {
		//NOTE: this only works for C++, if the interface is implemented directly in blueprint weird shit happens, look into it!
		ISpearableInterface* OverlappedSpearable = Cast<ISpearableInterface>(OtherActor);
		OverlappedSpearable->OnSpeared(this);
		CurrentlySpearedActor = OtherActor;
		
		TimeToPull = CurrentFishingEvent->GetTotalTimeToPull();
		QTEHandler->StartQTEs(CurrentFishingEvent, [this](bool bIsSuccessful) {this->OnQTEsResolved(bIsSuccessful);}, [this]() {this->OnQTEStart();}, [this]() {this->OnQTEEnd();});
		SpearedActorOriginalLocation = CurrentlySpearedActor->GetActorLocation();
		OnSpearingStarted();
		Spear->SetSimulatePhysics(false);
		Spear->AttachToComponent(CurrentlySpearedActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}


