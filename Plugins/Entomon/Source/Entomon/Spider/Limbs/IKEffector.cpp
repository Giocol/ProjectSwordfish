#include "IKEffector.h"
#include "Limb.h"

void FFootPlan::EvaluatePath(ULimb* InLimb, UPoseableMeshComponent* Mesh, float StepHeight, float InTraceDistance, 
	ECollisionChannel InTraceChannel) {
	FIKEffector MidWay = FIKEffector::Lerp(Start, Target, 0.5);
	FHitResult Hit;
	bool bHit = ULimb::TraceAround(InLimb, Mesh, MidWay.Location, -MidWay.UpVector, InTraceDistance, 16, InTraceChannel, Hit);
	
	if(bHit)
		MidPointOffset = MidWay.UpVector * StepHeight;
	else
		MidPointOffset = StepHeight * MidWay.GetUpVector();
}

void FFootPlan::LerpWithPeak(double Alpha) {
	float ParabolicAlpha = 4 * Alpha - 4 * Alpha * Alpha;
	Current.SetLocation(FMath::Lerp(Start.GetLocation(), Target.GetLocation(), Alpha) + MidPointOffset * ParabolicAlpha);
	Current.SetUpVector(FQuat::Slerp(Start.GetUpVector().ToOrientationQuat(), Target.GetUpVector().ToOrientationQuat(), Alpha).GetForwardVector());
}
