#pragma once

#include "CoreMinimal.h"

struct FNavLink {
	float Distance = 0;
	bool bTraversable = true;
	int Id = INDEX_NONE;
};

struct FNavNode {
	FNavNode() { }
	FNavNode(FVector Origin)
		: Origin(Origin) { }
	
	FORCEINLINE void Connect(FNavLink Link) {
		Connections.Add(Link);
	}
	
	FVector Origin = FVector::ZeroVector;
	FVector DisplacementFromNearestSurface = FVector::ZeroVector;
	float DistanceFromNearestSurface = 0;

	TArray<FNavLink> Connections;

	int Axis = 0;
};