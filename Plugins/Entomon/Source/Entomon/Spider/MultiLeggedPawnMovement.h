// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MultiLeggedPawnMovement.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ENTOMON_API UMultiLeggedPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UMultiLeggedPawnMovement();

	void AddAngularImpulse(FVector Impulse, bool bClamp = true);
	void AddInputRotation(FVector Input);
	void ApproachOrientation(FQuat InOrientation);

	virtual void AddRadialImpulse(const FVector& Origin, float Radius, float Strength, ERadialImpulseFalloff Falloff, bool bVelChange) override;

protected:
	virtual void BeginPlay() override;

	void ApplyControlRotationInputToAngularVelocity(float DeltaTime);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(Units="rad"))
		float MaxAngularSpeed = PI;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
		float AngularAcceleration = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
		bool bFaceTarget = true;

	FVector AngularVelocity = FVector::ZeroVector;
	

	FVector CurrentAngularInput = FVector::ZeroVector;
	FVector LastAngularInput = FVector::ZeroVector;
};
