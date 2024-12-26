// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiLeggedPawnMovement.h"


// Sets default values for this component's properties
UMultiLeggedPawnMovement::UMultiLeggedPawnMovement() {
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UMultiLeggedPawnMovement::AddAngularImpulse(FVector Impulse, bool bClamp) {
	AngularVelocity+=Impulse;
	if(bClamp && AngularVelocity.SquaredLength()>MaxAngularSpeed*MaxAngularSpeed)
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

void UMultiLeggedPawnMovement::AddInputRotation(FVector Input) {
	CurrentAngularInput += Input;
}

void UMultiLeggedPawnMovement::ApproachOrientation(FQuat DeltaRotation) {
	FVector AsRotationVector = DeltaRotation.ToRotationVector();
	FVector Axis;
	float AngularDistance;
	AsRotationVector.ToDirectionAndLength(Axis, AngularDistance);
	float DecelerationTime = MaxAngularSpeed / AngularAcceleration;
	float DecelerationAngle = DecelerationTime * MaxAngularSpeed / 2;
	float CurrentAngle = AngularDistance;
	float Ratio = FMath::Clamp(CurrentAngle / DecelerationAngle, 0.f, 1.f);
	FVector TargetAngularVelocity = Ratio * MaxAngularSpeed * Axis;
	FVector Diff = TargetAngularVelocity - AngularVelocity;
	FVector Input = Diff / AngularAcceleration;
	AddInputRotation(Input.GetClampedToMaxSize(1.f));
}

void UMultiLeggedPawnMovement::AddRadialImpulse(const FVector& Origin, float Radius, float Strength,
	ERadialImpulseFalloff Falloff, bool bVelChange) {

	
}


void UMultiLeggedPawnMovement::BeginPlay() {
	Super::BeginPlay();
	
}

void UMultiLeggedPawnMovement::ApplyControlRotationInputToAngularVelocity(float DeltaTime) {
	AddAngularImpulse(CurrentAngularInput.GetClampedToMaxSize(1.f) * AngularAcceleration);
	
}


// Called every frame
void UMultiLeggedPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ApplyControlRotationInputToAngularVelocity(DeltaTime);
	
	FQuat DeltaRotation = FQuat::MakeFromRotationVector(AngularVelocity * DeltaTime);
	GetOwner()->AddActorWorldRotation(DeltaRotation.Rotator());

	LastAngularInput = CurrentAngularInput;
	CurrentAngularInput = FVector::ZeroVector;
}

