﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimbSegment.generated.h"

USTRUCT(Blueprintable)
struct FLimbSegment {
	GENERATED_BODY()
	FLimbSegment() { }
	FLimbSegment(FName InName, FQuat InState)
		: Name(InName), RestState(InState), CurrentState(InState) { }

public:
	FName GetName() const { return Name; }
	FQuat GetRestState() const { return RestState; }
	FQuat GetState() const { return CurrentState; }
	void SetState(FQuat State) { CurrentState = State; }
	float GetLength() const { return Length; }
	void SetLength(float InLength) { Length = InLength; }
	
private:
	UPROPERTY(VisibleInstanceOnly)
	FName Name;
	UPROPERTY(VisibleInstanceOnly)
	FQuat RestState;
	UPROPERTY(VisibleInstanceOnly)
	FQuat CurrentState; // In component space.
	UPROPERTY(VisibleInstanceOnly)
	float Length = 0.f;
	
	FVector DummyLocation = FVector::ZeroVector; // In component space.
};