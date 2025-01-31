#pragma once

#include "CoreMinimal.h"
#include "INoiseMaker.h"
#include "GameFramework/Actor.h"
#include "ThrowableNoiseMaker.generated.h"

class USphereComponent;

UCLASS()
class PROJECTSWORDFISH_API AThrowableNoiseMaker : public AActor, public INoiseMaker {
	GENERATED_BODY()

public:
	AThrowableNoiseMaker();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit );

	UFUNCTION(BlueprintCallable)
		void Throw(float ThrowImpulse);
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* ThrowableMesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
		UNoiseDataAsset* NoiseDataAsset = nullptr;
	
	bool bHasMadeNoise = false;
};
