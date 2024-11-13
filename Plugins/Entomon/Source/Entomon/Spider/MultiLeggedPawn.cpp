#include "MultiLeggedPawn.h"

#include "GaitPreset.h"
#include "Components/BoxComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Limbs/ProceduralLimbManager.h"

AMultiLeggedPawn::AMultiLeggedPawn() {
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	Collision = CreateDefaultSubobject<UBoxComponent>("Collision");
	Collision->SetupAttachment(Root);
	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
	LimbManager = CreateDefaultSubobject<UProceduralLimbManager>("Limb Manager");
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	
	//LimbManager->AutoDetectLimbs(Mesh);
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiLeggedPawn::Move(FVector Target) {
	FVector ToTarget = TargetLocation - GetActorLocation();
	FVector ToTargetNorm = ToTarget.GetSafeNormal();
	float time = MovementComponent->MaxSpeed / MovementComponent->Deceleration;
	float dist = 0.5 * time * MovementComponent->MaxSpeed;
	FQuat DeltaQuat = FQuat::FindBetweenNormals(GetActorUpVector(), TargetNormal);
	FVector PlaneTarget = FVector::VectorPlaneProject(ToTargetNorm, GetActorUpVector()).GetSafeNormal();
	FQuat DeltaZRotation = FQuat::FindBetweenNormals(
		GetActorForwardVector(),
		PlaneTarget);
	
	AddActorWorldRotation(FQuat::Slerp(FQuat::Identity, DeltaQuat, 0.01f));
	AddActorWorldRotation(FQuat::Slerp(FQuat::Identity, DeltaZRotation, 0.02f));
	if(ToTarget.Length() < dist)
		return;
	float Dot = PlaneTarget.Dot(GetActorForwardVector());
	float Angle = FMath::Acos(Dot);
	float NormalizedFacingInverse = 1.f - Angle / TWO_PI;
	
	float InputModifier = FMath::Pow(NormalizedFacingInverse, (1-FacingBias) / FacingBias);
	MovementComponent->AddInputVector(InputModifier * ToTargetNorm);
}

void AMultiLeggedPawn::BeginPlay() {
	TargetLocation = GetActorLocation();
	LimbManager->AutoDetectLimbs(Mesh);
	ApplyGaitPreset(GaitPreset);
	Super::BeginPlay();
}

void AMultiLeggedPawn::Tick(float DeltaTime) {
	
	Move(TargetLocation);
}

void AMultiLeggedPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMultiLeggedPawn::ApplyGaitPreset(UGaitPreset* InGaitPreset) {
	if(!InGaitPreset)
		return;
	MovementComponent->MaxSpeed = InGaitPreset->MaxSpeed;
	MovementComponent->Acceleration = InGaitPreset->Acceleration;
	MovementComponent->Deceleration = InGaitPreset->Deceleration;
	LimbManager->ApplyGaitPreset(InGaitPreset);
}

