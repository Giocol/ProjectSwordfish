#pragma once

#include "CoreMinimal.h"
#include "Math/Bounds.h"

struct FBoundingVolume {
	FBoundingVolume() { }
	FBoundingVolume(int ParentId)
		: ParentId(ParentId) {}
	FBoundingVolume(int ParentId, FVector Extent, FVector Origin)
		: ParentId(ParentId), Extent(Extent), Origin(Origin) {}
	FBoundingVolume(FVector Extent, FVector Origin)
		: Extent(Extent), Origin(Origin) {}

	FORCEINLINE void SetParent(int Id) { ParentId = Id; }
	FORCEINLINE int GetParent() const { return ParentId; }
	FORCEINLINE void SetChildren(TArray<int> InChildren) { Children = InChildren; }
	FORCEINLINE TArray<int> GetChildren() const { return Children; }

	FORCEINLINE FVector GetExtent() const { return Extent; }
	FORCEINLINE FVector GetOrigin() const { return Origin; }
	
	FORCEINLINE bool IsRoot() const { return ParentId == INDEX_NONE; }
	FORCEINLINE double GetSignedDistance(FVector Point) const {
		FVector ToSurface = (Point - Origin).GetAbs() - Extent;
		double Result =
			FVector::Max(ToSurface, FVector::ZeroVector).Length() +
			FMath::Min(ToSurface.GetMax(), 0);
		return Result;
	}

private:
	int ParentId = INDEX_NONE;
	TArray<int> Children;
	FVector Extent = FVector::OneVector;
	FVector Origin = FVector::ZeroVector;
};
