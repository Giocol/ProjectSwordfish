#include "Spider.h"

#include "Components/PoseableMeshComponent.h"
#include "Legs/ProceduralLimbManager.h"

ASpider::ASpider() {

	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>("Mesh");
	RootComponent = Mesh;
	LimbManager = CreateDefaultSubobject<UProceduralLimbManager>("Limb Manager");
	PrimaryActorTick.bCanEverTick = true;
}

void ASpider::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

