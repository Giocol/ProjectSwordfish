// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiLeggedPawnMovement.h"


// Sets default values for this component's properties
UMultiLeggedPawnMovement::UMultiLeggedPawnMovement() {
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UMultiLeggedPawnMovement::AddAngularImpulse(FVector Impulse) {
	AngularVelocity+=Impulse;
	if(AngularVelocity.SquaredLength()>MaxAngularSpeed*MaxAngularSpeed)
		AngularVelocity = AngularVelocity.GetClampedToMaxSize(MaxAngularSpeed);
	// FVector RotationVector = CombinedRotation.ToRotationVector();
	// float RotationAngle;
	// RotationVector.ToDirectionAndLength(RotationVector, RotationAngle);
	// float NewRotationAngle = FMath::Min(DeltaTime * RotationSpeed, RotationAngle);
	// RotationVector = RotationVector * NewRotationAngle;
	// FQuat Rotation = FQuat::MakeFromRotationVector(RotationVector);
	//
	// FVector RelativeLocation = Body->GetComponentLocation() - GetActorLocation();
	// FVector Offset = RelativeLocation - Rotation * RelativeLocation;
	// AddActorWorldRotation(Rotation);
	// AddActorWorldOffset(Offset);
}

void UMultiLeggedPawnMovement::AddInputRotation(FVector Input) { AddAngularImpulse(Input * AngularAcceleration); }


void UMultiLeggedPawnMovement::BeginPlay() {
	Super::BeginPlay();
	
}


// Called every frame
void UMultiLeggedPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FQuat DeltaRotation = FQuat::MakeFromRotationVector(AngularVelocity * DeltaTime);
	GetOwner()->AddActorWorldRotation(DeltaRotation.Rotator());
}

