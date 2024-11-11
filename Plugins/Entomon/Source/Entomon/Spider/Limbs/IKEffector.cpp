#include "IKEffector.h"
#include "Limb.h"

void FFootPlan::Initialize(UPoseableMeshComponent* Mesh, float StepHeight, float InTraceDistance,
	ECollisionChannel InTraceChannel) {
	FIKEffector MidWay = FIKEffector::Lerp(Start, Target, 0.5);
	FHitResult Hit;
	bool bHit = ULimb::TraceFoot(Mesh, MidWay.Location + MidWay.UpVector * InTraceDistance,
		-MidWay.UpVector, InTraceDistance * 2, InTraceChannel, Hit);
	
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
