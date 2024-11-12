#include "MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "ProjectSwordfish/Environment/InteractableInterface.h"
#include "ProjectSwordfish/Environment/SpearableInterface.h"
#include "..\UI\SliderWithTargetAreas.h"


AMainCharacter::AMainCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(RootComponent);
	Spear = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spear Mesh"));
	Spear->SetupAttachment(Camera);
	Spear->OnComponentHit.AddDynamic(this, &AMainCharacter::OnSpearHit);
	PullTarget = CreateDefaultSubobject<USceneComponent>(TEXT("Pull Target"));
	PullTarget->SetupAttachment(RootComponent);
}

void AMainCharacter::ProcessCharacterMovementInput(const FVector2D input) {
	if(!bIsFishing) {
		AddMovementInput(GetActorForwardVector(), input.X);
		AddMovementInput(GetActorRightVector(), input.Y);
	}
}

void AMainCharacter::ProcessCameraMovementInput(const FVector2D Input) {
	if(!bIsFishing) {
		AddControllerPitchInput(Input.X);
		AddControllerYawInput(Input.Y);
	} else {
		//TODO: THIS IS FRAMERATE DEPENDANT!! FIX!!!!!
		CurrentAim = FMath::Clamp((CurrentAim + FMath::Sign(Input.Y) * AimStep), 0.f, 1.f);
	}
}

void AMainCharacter::ProcessInteract() {
	FHitResult HitResult;
	TraceInteract(HitResult);

	if(HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractableInterface>())
	{
		IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitResult.GetActor());
		Interactable->Interact(this);
	}
}

void AMainCharacter::ProcessUse() {
	if(bHasSpear && bIsFishing) {
		if(bIsAimInThreshold && bIsPowerInGoodThreshold) // todo: expand this condition, add timer before being able to throw
			UE_LOG(LogTemp, Warning, TEXT("YOU AIMED RIGHT!!!!"));
	}
	//if(bHasSpear)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("I'm throwing the spear!"));
	//	//TODO: we're not gonna use physics later, it was just to get it done quick and dirty
	//	Spear->SetSimulatePhysics(true);
	//	Spear->AddImpulse(Spear->GetForwardVector()*SpearSpeed);
	//}
}

FFishingSliderData AMainCharacter::GetFishingSliderData(EFishingSliderType Type) const {
	if(Type == EFishingSliderType::Aiming) {
		return FFishingSliderData(CurrentAim,
			FVector2d(AimLowerThreshold, AimUpperThreshold));
	} else if(Type == EFishingSliderType::Power) {
		return FFishingSliderData(CurrentPower,
			FVector2d(PowerGoodLowerThreshold, PowerGoodUpperThreshold),
			FVector2d(PowerMediumLowerThreshold, PowerMediumUpperThreshold));
	} else { //Default case, should never occur
		UE_LOG(LogTemp, Error, TEXT("EFishingSliderType passed was None! Something went wrong!"));
		return FFishingSliderData();
	}
}

float AMainCharacter::GetFishingSliderValue(EFishingSliderType Type) const {
	if(Type == EFishingSliderType::Aiming) {
		return CurrentAim;
	} else if(Type == EFishingSliderType::Power) {
		return CurrentPower;
	} else { //Default case, should never occur
		UE_LOG(LogTemp, Error, TEXT("EFishingSliderType passed was None! Something went wrong!"));
		return -1;
	}
}

void AMainCharacter::Pull(float DeltaTime) {
	//todo: this is a terrible name for the function
	if(bIsFishing) {
		CurrentPower = FMath::Clamp(CurrentPower + PowerStep * DeltaTime, 0.0f, 1.f);
	}
	////todo: don't move on z
	//CurrentlySpearedActor->SetActorLocation(
	//	FMath::Lerp(
	//		CurrentlySpearedActor->GetActorLocation(),
	//		PullTarget->GetComponentLocation(),
	//		DeltaTime *PullSpeed));
//
	////todo: maybe detect overlap with a SuccessfulPullBox or smth rather than this hacky math
	//if((CurrentlySpearedActor->GetActorLocation() - PullTarget->GetComponentLocation()).Length() < 10.f) {
	//	if(!CurrentlySpearedActor->Implements<USpearableInterface>())
	//		return;
	//	//NOTE: this only works for C++, if the interface is implemented directly in blueprint weird shit happens, look into it!
	//	ISpearableInterface* OverlappedSpearable = Cast<ISpearableInterface>(CurrentlySpearedActor);
	//	OverlappedSpearable->OnPullCompleted(this);
	//	CurrentlySpearedActor = nullptr;
	//}
}

void AMainCharacter::OnSpearHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit) {

	if(OtherActor->Implements<USpearableInterface>()) {
		//NOTE: this only works for C++, if the interface is implemented directly in blueprint weird shit happens, look into it!
		ISpearableInterface* OverlappedSpearable = Cast<ISpearableInterface>(OtherActor);
		OverlappedSpearable->OnSpeared(this);
		CurrentlySpearedActor = OtherActor;

		Spear->SetSimulatePhysics(false);
		Spear->AttachToComponent(CurrentlySpearedActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AMainCharacter::BeginPlay() {
	Spear->SetVisibility(false);
	
	Super::BeginPlay();
}

void AMainCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(bIsFishing) {
		FishingTick(DeltaTime);
	}
	
	if(bIsPulling && bIsFishing)
		Pull(DeltaTime);
}

void AMainCharacter::FishingTick(float DeltaTime) {
	if(AimLowerThreshold < CurrentAim && CurrentAim < AimUpperThreshold) {
		bIsAimInThreshold = true;
		UE_LOG(LogTemp, Error, TEXT("AIM"));
	}
	else {
		bIsAimInThreshold = false;
	}

	if(PowerGoodLowerThreshold < CurrentPower && CurrentPower < PowerGoodUpperThreshold) {
		bIsPowerInGoodThreshold = true;
		UE_LOG(LogTemp, Error, TEXT("Power good"));
	}
	else {
		bIsPowerInGoodThreshold = false;
	}

	if(PowerMediumLowerThreshold < CurrentPower && CurrentPower < PowerMediumUpperThreshold) {
		bIsPowerInMediumThreshold = true;
		UE_LOG(LogTemp, Error, TEXT("Power medium"));
	}
	else {
		bIsPowerInMediumThreshold = false;
	}

	ApplyFishingResistance(DeltaTime);
	CurrentPower = FMath::Clamp(CurrentPower - PowerDecayPerTick * DeltaTime, 0.f, 1.f);
}

void AMainCharacter::ApplyFishingResistance(float DeltaTime) {
	if(CurrentAim < AimLowerThreshold + (AimUpperThreshold - AimLowerThreshold)/2)
		CurrentAim = FMath::Clamp(CurrentAim - AimResistancePerTick * DeltaTime, 0.f, 1.f);
	else
		CurrentAim = FMath::Clamp(CurrentAim + AimResistancePerTick * DeltaTime, 0.f, 1.f);
}

void AMainCharacter::TraceInteract(FHitResult& OutHitResult) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	FRotator StartRotation;
	FVector StartLocation;

	GetController()->GetPlayerViewPoint(StartLocation, StartRotation);
	const FVector EndLocation = StartLocation + StartRotation.Vector() * InteractionRange;

	GetWorld()->LineTraceSingleByChannel
	(
		OutHitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);
}

