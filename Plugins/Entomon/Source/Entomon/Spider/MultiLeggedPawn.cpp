#include "MultiLeggedPawn.h"

#include "Components/BoxComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Limbs/ProceduralLimbManager.h"

AMultiLeggedPawn::AMultiLeggedPawn() {
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	Collision = CreateDefaultSubobject<UBoxComponent>("Collision");
	Collision->SetupAttachment(Root);
	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
	LimbManager = CreateDefaultSubobject<UProceduralLimbManager>("Limb Manager");
	//LimbManager->AutoDetectLimbs(Mesh);
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiLeggedPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMultiLeggedPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMultiLeggedPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

