// Fill out your copyright notice in the Description page of Project Settings.


#include "LimbSegment.h"

void ULimbSegment::Initialize(FName InName, FQuat InState) {
	Name = InName;
	CurrentState = InState;
	RestState = InState;
}
