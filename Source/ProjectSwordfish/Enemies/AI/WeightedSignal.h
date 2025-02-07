#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerceptionSignal.h"

#include "WeightedSignal.generated.h"

class AEnemyBase;

USTRUCT(BlueprintType)
struct FWeightedSignal {
	GENERATED_BODY()
    
	FWeightedSignal() { }
	FWeightedSignal(FPerceptionSignal Signal)
		: Signal(Signal) {
		if(!Signal.Actor) Weight = Signal.SignalStrength;
	}

	UPROPERTY(BlueprintReadOnly, Category = "AI|Perception|Signal")
	FPerceptionSignal Signal;
	UPROPERTY(BlueprintReadOnly, Category = "AI|Perception|Signal")
	float Weight = 0.f;
	
	bool bPositiveSlopeSign = true;
	
	FORCEINLINE void ResetDecay() noexcept { if(Weight < 1.f) Weight = 1.f; }
	FORCEINLINE float GetWeight() const noexcept {
		return Weight;
	}
	FORCEINLINE bool IsResolvable() const noexcept { return !bPositiveSlopeSign; }
	
	FORCEINLINE bool operator ==(FWeightedSignal Other) const noexcept {
		if(Signal.Actor)
			return Signal.Actor == Other.Signal.Actor;
		else {
			return Signal.SignalOrigin == Other.Signal.SignalOrigin;
		}
	}
	
	FORCEINLINE bool IsValid() const {
		return *this != FWeightedSignal();
	}
};


USTRUCT(BlueprintType, Blueprintable)
struct FWeightedClass {
	GENERATED_BODY()

	FWeightedClass() {}
	FWeightedClass(TSubclassOf<AActor> Class, float Weight)
		: Class(Class), Weight(Weight) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 1.f;
};