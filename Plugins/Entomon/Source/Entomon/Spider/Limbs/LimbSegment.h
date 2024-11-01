// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimbSegment.generated.h"

UCLASS(Blueprintable)
class ULimbSegment : public UObject {
	GENERATED_BODY()
	ULimbSegment() {}

public:
	void Initialize(FName InName, FQuat InState);
	
	FName GetName() { return Name; }
	FQuat GetRestState() { return RestState; }
	FQuat GetState() { return CurrentState; }
	void SetState(FQuat State) { CurrentState = State; }
	
private:
	UPROPERTY(VisibleInstanceOnly)
	FName Name;
	UPROPERTY(VisibleInstanceOnly)
	FQuat RestState;
	UPROPERTY(VisibleInstanceOnly)
	FQuat CurrentState; // In component space.
	
	float Length = 0.f;
};