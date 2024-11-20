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

bool AMultiLeggedPawn::Move(double DeltaTime, FNavNode Target) {
	FVector ToTarget = Target.Origin - GetActorLocation();
	FVector ToTargetNorm = ToTarget.GetSafeNormal();
	float time = MovementComponent->MaxSpeed / MovementComponent->Deceleration;
	float brakeDistance = 0.5 * time * MovementComponent->MaxSpeed;
	// FVector PlaneTarget = FVector::VectorPlaneProject(ToTargetNorm, GetActorUpVector()).GetSafeNormal();

	if(ToTarget.Length() < brakeDistance)
		return true;
	// float Dot = PlaneTarget.Dot(GetActorForwardVector());
	// float Angle = FMath::Acos(Dot);
	// float NormalizedFacingInverse = 1.f - Angle / TWO_PI;
	
	// float InputModifier = FMath::Pow(NormalizedFacingInverse, (1-FacingBias) / FacingBias);
	MovementComponent->AddInputVector(ToTargetNorm);
	
	return false;
}

void AMultiLeggedPawn::Rotate(double DeltaTime, FNavNode Target) {
	FVector ToTarget = Target.Origin - GetActorLocation();
	FVector ToTargetNorm = ToTarget.GetSafeNormal();

	FQuat DeltaQuat = FQuat::FindBetweenNormals(GetActorUpVector(), Target.Normal);
	FVector PlaneTarget = FVector::VectorPlaneProject(ToTargetNorm, GetActorUpVector()).GetSafeNormal();
	FQuat DeltaZRotation = FQuat::FindBetweenNormals(
		GetActorForwardVector(),
		PlaneTarget);
	
	AddActorWorldRotation(FQuat::Slerp(FQuat::Identity, DeltaQuat, DeltaTime * RotationSpeed));
	// AddActorWorldRotation(FQuat::Slerp(FQuat::Identity, DeltaZRotation, 2 * DeltaTime * RotationSpeed));
}

void AMultiLeggedPawn::FollowPath(double DeltaTime) {
	if(CurrentPathId < Path.Num()-1) {
		Rotate(DeltaTime, Path[CurrentPathId+1]);
		if(Move(DeltaTime, Path[CurrentPathId+1])) {
			CurrentPathId++;
		}
	}
	else if(CurrentPathId == Path.Num()-1)
		Rotate(DeltaTime, Path[CurrentPathId]);
}

void AMultiLeggedPawn::BeginPlay() {
	LimbManager->AutoDetectLimbs(Mesh);
	ApplyGaitPreset(GaitPreset);
	Super::BeginPlay();
}

void AMultiLeggedPawn::Tick(float DeltaTime) {
	
	FollowPath(DeltaTime);

	for(int i = 0; i < Path.Num() - 1; ++i)
		DrawDebugLine(GetWorld(), Path[i].Origin, Path[i+1].Origin, FColor::Yellow);
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

