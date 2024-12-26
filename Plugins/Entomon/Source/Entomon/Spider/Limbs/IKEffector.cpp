#include "IKEffector.h"
#include "Limb.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void FFootPlan::EvaluatePath(ULimb* InLimb, UPoseableMeshComponent* Mesh, USceneComponent* Root, float StepHeight, 
                             ECollisionChannel InTraceChannel) {
	FIKEffector MidWay = FIKEffector::Lerp(Start, Target, 0.5);
	FHitResult Hit;
	bool bHit = ULimb::TraceAround(InLimb, Mesh, Root, MidWay.Location, -MidWay.UpVector,
		16, InTraceChannel, MidWay.Location, Hit);
	
	if(bHit)
		MidPointOffset = MidWay.UpVector * StepHeight;
	else
		MidPointOffset = StepHeight * MidWay.GetUpVector();
}

void FFootPlan::LerpWithPeak(UPoseableMeshComponent* InMesh, float& Alpha, ECollisionChannel InTraceChannel) {
	float ParabolicAlpha = 4 * Alpha - 4 * Alpha * Alpha;
	FVector NewLocation = FMath::Lerp(Start.GetLocation(), Target.GetLocation(), Alpha) + MidPointOffset * ParabolicAlpha;
	// TArray<AActor*> Ignore = { InMesh->GetOwner() };
	// FHitResult Hit;
	// bool bHit = UKismetSystemLibrary::LineTraceSingle(
	// 	InMesh->GetWorld(),
	// 	Current.GetLocation(), NewLocation,
	// 	UEngineTypes::ConvertToTraceType(InTraceChannel),
	// 	false, Ignore, EDrawDebugTrace::ForOneFrame,
	// 	Hit, false
	// 	);
	// if(bHit && Hit.Distance > 0) {
	// 	Target.SetLocation(Hit.ImpactPoint);
	// 	Target.SetUpVector(Hit.ImpactNormal);
	// 	Alpha = 1;
	// 	NewLocation = Hit.ImpactPoint;
	// }
	Current.SetLocation(NewLocation);
	Current.SetUpVector(FQuat::Slerp(Start.GetUpVector().ToOrientationQuat(), Target.GetUpVector().ToOrientationQuat(), Alpha).GetForwardVector());
}
