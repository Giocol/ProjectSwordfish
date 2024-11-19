#pragma once

#include "CoreMinimal.h"

#include "NavNode.generated.h"

struct FNavLink {
	FNavLink(int Id, float Distance) : Id(Id) {}
	float Distance = 0;
	bool bTraversable = true;
	int Id = INDEX_NONE;

	const bool operator==(const FNavLink& Other) {
		return Id == Other.Id;
	}
};

USTRUCT(BlueprintType)
struct FNavNode {
	GENERATED_BODY()
	FNavNode() { }
	FNavNode(FVector Origin)
		: Origin(Origin) { }
	
	FORCEINLINE void Connect(FNavLink Link) {
		Connections.Add(Link);
	}
	
	FVector Origin = FVector::ZeroVector;
	FVector Normal = FVector::ZeroVector;
	float Distance = 0;

	TArray<FNavLink> Connections;
};