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
	
	void ProcessCharacterMovementInput(FVector2D input);
	void ProcessCameraMovementInput(FVector2D Input);
	void ProcessInteract();
	void ProcessUse();

	void SetHasSpear(bool State) { bHasSpear = State; Spear->SetVisibility(true); };
	
	UFUNCTION()
	void OnSpearHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void TraceInteract(FHitResult& OutHitResult) const;

protected:
	UPROPERTY(EditDefaultsOnly)
		UCameraComponent* Camera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float InteractionRange = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		UStaticMeshComponent* Spear = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fishing")
		float SpearSpeed = 10.f;

	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		AActor* CurrentlySpearedActor = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Fishing")
		bool bHasSpear = false;
};
