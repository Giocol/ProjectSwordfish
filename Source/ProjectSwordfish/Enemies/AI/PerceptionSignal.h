#pragma once

#include "CoreMinimal.h"
#include "PerceptionSignal.generated.h"

USTRUCT(BlueprintType)
struct FPerceptionSignal {
	GENERATED_BODY()

	FPerceptionSignal() { Actor = nullptr; SignalStrength = 0.f; SignalOrigin = FVector::ZeroVector; }
	FPerceptionSignal(float SignalStrength, AActor* Actor)
		:  SignalStrength(SignalStrength), Actor(Actor) { SignalOrigin = Actor->GetActorLocation(); }
	FPerceptionSignal(float SignalStrength, FVector Origin)
		:  SignalStrength(SignalStrength), SignalOrigin(Origin) { }

	float SignalStrength;
	AActor* Actor = nullptr;
	FVector SignalOrigin;

	FORCEINLINE bool operator==(FPerceptionSignal Other) {
		return SignalOrigin == Other.SignalOrigin || ((Actor || Other.Actor) && Actor == Other.Actor);
	} 
};

UCLASS()
class USignalFunctions : public UObject {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AI|Perception")
	static FVector GetSignalOrigin(FPerceptionSignal Signal) noexcept { return Signal.SignalOrigin; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AI|Perception")
	static AActor* GetActor(FPerceptionSignal Signal) noexcept { return Signal.Actor; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AI|Perception")
	static float GetSignalStrength(FPerceptionSignal Signal) noexcept { return Signal.SignalStrength; }
	
};