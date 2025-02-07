#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "SightComponent.generated.h"

UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API USightComponent : public UStaticMeshComponent {
	GENERATED_BODY()
public:
	USightComponent();

	bool IsActorVisible(AActor* Actor, float& SignalStrength) const;
	bool IsLocationVisible(FVector Location, float Tolerance, float& SignalStrength) const;
	TArray<AActor*> GetAllVisibleActors() const;

	FORCEINLINE TArray<AActor*> GetActorsInVisionCone() const noexcept { return ActorsInVisionCone; }
	bool IsLocationInVisionCone(FVector Location) const noexcept;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Perception)
	float DetectionRate = 1.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Perception)
	float DetectionFalloffPower = 0.0;

protected:
	virtual void BeginPlay() override;
	
	virtual void UpdateVisionMeshScale();
	void SetDefaultCollisionResponse();
	
	float EvaluateSignalStrength(float Distance) const;
	float EvaluateSignalStrength(FVector Location) const;
	float EvaluateSignalStrength(AActor* Actor) const;

	FHitResult TraceTo(FVector Location) const;
	
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, 
					  AActor* OtherActor, 
					  UPrimitiveComponent* OtherComponent, 
					  int32 OtherBodyIndex, 
					  bool bFromSweep, 
					  const FHitResult &SweepResult );
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, 
					  AActor* OtherActor, 
					  UPrimitiveComponent* OtherComponent, 
					  int32 OtherBodyIndex);
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	UPROPERTY(EditDefaultsOnly, Category = Perception)
	float ViewDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = Perception, meta = (UIMin = 0.f, UIMax = 89.9f));
	float ViewAngle = 45.f;
	
	UPROPERTY(EditDefaultsOnly, Category = Perception)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;

	TArray<AActor*> ActorsInVisionCone;
};
