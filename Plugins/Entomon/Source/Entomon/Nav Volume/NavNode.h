#pragma once

#include "CoreMinimal.h"

struct FNavNode {
	FNavNode() { }
	FNavNode(FVector Origin)
		: Origin(Origin) { }
	
	FVector Origin = FVector::ZeroVector;
	float DistanceToNearestSurface = INFINITY;

	TArray<int> Connections;
};