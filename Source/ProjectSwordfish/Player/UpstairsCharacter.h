#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "ProjectSwordfish/UI/SliderWithTargetAreas.h"
#include "UpstairsCharacter.generated.h"

class UFishingEventDataAsset;
///<summary>
/// All values are supposed to be contained between 0 and 1. A value of -1 signifies that that specific property is invalid
///</summary>
USTRUCT()
struct FFishingSliderData
{
	GENERATED_BODY()
	float Value = -1;
	FVector2d GoodTargetAreaBounds = FVector2d::One() * -1;
	FVector2d MediumTargetAreaBounds = FVector2d::One() * -1;
};

UCLASS()
class PROJECTSWORDFISH_API AUpstairsCharacter : public AMainCharacter {
	GENERATED_BODY()

public:
	AUpstairsCharacter();
	virtual void BeginPlay() override;

	//todo: add check for having spear
	UFUNCTION(BlueprintCallable)
	bool StartFishingEvent(UFishingEventDataAsset* FishingEventData);

	UFUNCTION(BlueprintImplementableEvent)
	void OnFishingStarted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSpearingStarted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFishingEnded();

	UFUNCTION()
	void OnSpearHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	///<summary>
	/// All values are supposed to be contained between 0 and 1. A value of -1 signifies that that specific property is invalid
	///</summary>
	FFishingSliderData GetFishingSliderData(EFishingSliderType Type) const; //todo: kinda sucks? maybe pass a reference to an already built struct and repopulate it?
	float GetFishingSliderValue(EFishingSliderType Type) const;
	
	void SetHasSpear(bool State) { bHasSpear = State; Spear->SetVisibility(State);};

	//input handling
	virtual void ProcessCharacterMovementInput(FVector2D input) override;
	virtual void ProcessCameraMovementInput(FVector2D Input) override;
	virtual void Pull(float DeltaTime) override;
	virtual void ProcessUse() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	void FishingTick(float DeltaTime);
	void ApplyFishingResistance(float DeltaTime);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Fishing ")
		UStaticMeshComponent* Spear = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing ")
		bool bIsFishing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
		float CurrentAim = 0.f; //todo: clamp here?

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
			bool bIsAimInThreshold = false;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing | Aiming")
		float AimStep = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing | Aiming")
		float AimResistancePerTick = 0.01f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
		float CurrentPower = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
		bool bIsPowerInGoodThreshold = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
		bool bIsPowerInMediumThreshold = false;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing | Aiming")
		float PowerStep = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fishing | Aiming")
		float PowerDecayPerTick = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		USceneComponent* PullTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float SpearSpeed = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float PullSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		AActor* CurrentlySpearedActor = nullptr;
	
	
	
	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		bool bHasSpear = false;

	UPROPERTY()
		UFishingEventDataAsset* CurrentFishingEvent = nullptr;
};	
