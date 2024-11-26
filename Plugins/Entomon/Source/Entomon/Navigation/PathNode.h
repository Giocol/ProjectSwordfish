#pragma once

#include "CoreMinimal.h"
#include "NavNode.h"

#include "PathNode.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FPathNode {
	GENERATED_BODY()

	FPathNode() { }
	FPathNode(FNavNode Node) : Origin(Node.Origin), Normal(Node.Normal), SurfaceDistance(Node.Distance) { }
	FPathNode(FNavNode Node, FVector Next)
		: FPathNode(Node) {
		if(Next!=FVector::ZeroVector) {
			FVector ToNext = Next - Node.Origin;
			ToNext.ToDirectionAndLength(Tangent, NextNodeDistance);
		}
	}

	FVector Origin = FVector::ZeroVector;
	FVector Normal = FVector::ZeroVector;
	FVector Tangent = FVector::ZeroVector;
	float SurfaceDistance = 0;
	float NextNodeDistance = 0;
};