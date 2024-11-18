#pragma once

#include "CoreMinimal.h"

struct FNavLink {
	FNavLink(int Id, float Distance) : Id(Id) {}
	float Distance = 0;
	bool bTraversable = true;
	int Id = INDEX_NONE;

	const bool operator==(const FNavLink& Other) {
		return Id == Other.Id;
	}
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
};