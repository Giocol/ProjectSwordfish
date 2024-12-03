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

	void AddAngularImpulse(FVector Impulse);
	void AddInputRotation(FVector Input);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(Units="rad"))
		float MaxAngularSpeed = PI;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
		float AngularAcceleration = 100;

	FVector AngularVelocity = FVector::ZeroVector;
};
