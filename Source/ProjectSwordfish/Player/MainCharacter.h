// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class ISpearableInterface;
class UCameraComponent;

UCLASS()
class PROJECTSWORDFISH_API AMainCharacter : public ACharacter {
	GENERATED_BODY()

public:
	AMainCharacter();

	UFUNCTION(BlueprintCallable)
	void SetIsFishing(bool State) { bIsFishing = State;};
	
	void ProcessCharacterMovementInput(FVector2D input);
	void ProcessCameraMovementInput(FVector2D Input);
	void ProcessInteract();
	void ProcessUse();

	float GetCurrentAim() const { return  CurrentAim; };
	float GetCurrentPower() const { return  CurrentPower; };

	void SetHasSpear(bool State) { bHasSpear = State; Spear->SetVisibility(State); };
	void SetIsPulling(bool State) { bIsPulling = State; }

	void Pull(float DeltaTime);
	
	UFUNCTION()
	void OnSpearHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void FishingTick(float DeltaTime);
	void ApplyFishingResistance(float DeltaTime);

private:
	void TraceInteract(FHitResult& OutHitResult) const;

protected:
	UPROPERTY(EditDefaultsOnly)
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
		float InteractionRange = 20.f;
	
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
		float AimResistancePerTick = 0.01f; //TODO: let fish set this
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing | Aiming", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float AimLowerThreshold = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing | Aiming",  meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float AimUpperThreshold = 0.4f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
		float CurrentPower = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing | Aiming")
		bool bIsPowerInThreshold = false;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing | Aiming")
		float PowerStep = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fishing | Aiming")
		float PowerDecayPerTick = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing | Aiming", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float PowerLowerThreshold = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing | Aiming",  meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float PowerUpperThreshold = 0.4f;

	//todo: old stuff
	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		USceneComponent* PullTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float SpearSpeed = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float PullSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		AActor* CurrentlySpearedActor = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		bool bIsPulling = false;
	//todo: end of old stuff

	
	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		bool bHasSpear = false;
};
