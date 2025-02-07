#include "ThrowableNoiseMaker.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectSwordfish/Systems/DownstairsGameMode.h"

AThrowableNoiseMaker::AThrowableNoiseMaker() {
	ThrowableMesh = CreateDefaultSubobject<UStaticMeshComponent>("Sphere Collider");
	ThrowableMesh->SetupAttachment(RootComponent);
	ThrowableMesh->OnComponentHit.AddDynamic(this, &AThrowableNoiseMaker::OnHit);
}

void AThrowableNoiseMaker::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if(!bHasMadeNoise) {
		GenerateNoise(NoiseDataAsset, Hit.Location);
		bHasMadeNoise = true;
	}
}

void AThrowableNoiseMaker::Throw(float ThrowImpulse) {
	ThrowableMesh->SetSimulatePhysics(true);
	ThrowableMesh->AddImpulse(GetActorForwardVector()*ThrowImpulse);
}

void AThrowableNoiseMaker::BeginPlay() {
	Super::BeginPlay();

	ADownstairsGameMode* GameMode = Cast<ADownstairsGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(GameMode)
		NoiseSystemRef = GameMode->GetNoiseSystemRef();

	ThrowableMesh->SetSimulatePhysics(false);
}
