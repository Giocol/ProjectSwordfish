#pragma once

#include "CoreMinimal.h"
#include "Limb.generated.h"

USTRUCT(Blueprintable)
struct FBoneTransformPair {
	GENERATED_BODY()
	FBoneTransformPair() {}
	FBoneTransformPair(FName Name, FRotator State)
		: Name(Name), State(State) { };

	UPROPERTY(VisibleInstanceOnly)
	FName Name;
	UPROPERTY(VisibleInstanceOnly)
	FRotator State;
};
USTRUCT(Blueprintable)
struct FLeg {
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly)
	TArray<FBoneTransformPair> Bones;
	UPROPERTY(VisibleInstanceOnly)
	FVector Target;

};
