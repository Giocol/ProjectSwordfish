#pragma once

#include "CoreMinimal.h"
#include "IKEffector.generated.h"

USTRUCT()
struct FIKEffector {
	GENERATED_BODY();
	
	FIKEffector() { }
	FIKEffector(FVector InLocation, FQuat InRotation)
		: Location(InLocation) {
		UpVector = InRotation.GetUpVector();
	}
	FIKEffector(FVector InLocation, FVector InUpVector)
		: Location(InLocation), UpVector(InUpVector) { }

	FVector GetLocation() const noexcept { return Location; }
	void SetLocation(const FVector& InLocation) noexcept { Location = InLocation; }

	FVector GetUpVector() const noexcept { return UpVector; }
	void SetUpVector(const FVector& InVector) noexcept { UpVector = InVector; }

	
	
	static FIKEffector Lerp(FIKEffector a, FIKEffector b, double alpha) {
		FIKEffector Result = a;
		Result.SetLocation(FMath::Lerp(a.GetLocation(), b.GetLocation(), alpha));
		Result.SetUpVector(FQuat::Slerp(a.GetUpVector().ToOrientationQuat(), b.GetUpVector().ToOrientationQuat(), alpha).GetForwardVector());
		return Result;
	}
	
	FVector Location = FVector::ZeroVector;
	
	FVector UpVector = FVector::UpVector;
};

USTRUCT()
struct FFootPlan {
	GENERATED_BODY()

	FIKEffector Start;
	FIKEffector Current;
	FIKEffector Target;
	
	FVector MidPointOffset = FVector::ZeroVector;
	
	void EvaluatePath(class ULimb* InLimb, class UPoseableMeshComponent* Mesh, float StepHeight, float InTraceDistance, ECollisionChannel InTraceChannel);
	void LerpWithPeak(double Alpha);
};
