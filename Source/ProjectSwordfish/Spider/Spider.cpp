#include "Spider.h"

#include "Components/BoxComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Legs/ProceduralLimbManager.h"

ASpider::ASpider() {
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

