#include "Limb.h"

#include "LimbSegment.h"
#include "Components/PoseableMeshComponent.h"
#include "Entomon/Spider/GaitPreset.h"
#include "Kismet/KismetSystemLibrary.h"

void ULimb::Tick(UPoseableMeshComponent* Mesh, float DeltaTime, ECollisionChannel InTraceChannel) {
	if(bIsGrounded)
		return;

	FootPlan.LerpWithPeak(StepTimer);
	
	StepTimer += DeltaTime / StepDuration;
	if(StepTimer > 1.f) {
		StepTimer = 0;
		bIsGrounded = true;

		// TArray<FVector> JointLocations;
		// InitializeIK(Mesh, JointLocations);
		// EvaluateAngles(Mesh, JointLocations);
	}
}

bool ULimb::Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor) {
	if(!Mesh) return false;
	
	Joints.Add(MakeJoint(Mesh, EndEffectorName, true));
	FName CurrentBone = EndEffectorName;
	while(true) {
		CurrentBone = Mesh->GetParentBone(CurrentBone);
		if(CurrentBone.IsNone())
			return false;
		auto boneAsString = CurrentBone.ToString();
		auto templateName = HipNameToSearchFor.ToString();
		auto s = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		Joints.Add(MakeJoint(Mesh, CurrentBone));
		if(s != INDEX_NONE)
			break;
	}
	for(int i = 1; i < Joints.Num(); i++) {
		FVector CurrentLocation = Mesh->GetBoneLocationByName(Joints[i].GetName(), EBoneSpaces::WorldSpace);
		FVector NextLocation = Mesh->GetBoneLocationByName(Joints[i-1].GetName(), EBoneSpaces::WorldSpace);
		FVector Offset = NextLocation - CurrentLocation;
		float Length = Offset.Length();
		Joints[i].SetLength(Length);
		MaxLength += Length;
	}
	HipLocation = Mesh->GetBoneLocationByName(Joints.Last().GetName(), EBoneSpaces::ComponentSpace);
	// Pole
	
	
	return true;
}

void ULimb::UpdateIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw) {
	auto it = Solve_FABRIK(Mesh, Threshold, Iterations);
	//if(it > 0)
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("%i"), it));
	if (bDraw) {
		DrawIK(Mesh, Threshold);
	}
}

bool ULimb::TryMove(UPoseableMeshComponent* InMesh, float GaitCycleDuration, int Iterations,
		ECollisionChannel InTraceChannel) {
	if (!EvaluateTargetPosition(this, InMesh, GaitCycleDuration, MaxLength, Iterations, InTraceChannel))
		return false;
	if(FVector::Distance(InMesh->GetBoneLocationByName(Joints[0].GetName(), EBoneSpaces::WorldSpace), FootPlan.Current.GetLocation()) < 10.f
			&& FVector::Distance(FootPlan.Current.GetLocation(), FootPlan.Target.GetLocation()) < 10.f)
		return true;
	FootPlan.Start = FootPlan.Current;
	FootPlan.EvaluatePath(this, InMesh, StepHeight, InTraceChannel);
	bIsGrounded = false;
	return true;
	//CurrentIK.SetLocation(Target);
	//bHasRelocated = true;
}

void ULimb::ApplyGaitPreset(UGaitPreset* InGaitPreset) {

	StepDuration = InGaitPreset->StepDuration;
	StepHeight = InGaitPreset->StepHeight;
	
	for(auto Gait : InGaitPreset->PerLimbGaitInfo) {
		auto name = GetName();
		bool foundName = false;
		bool foundLocator = false;

		for (auto Name : Gait.Keywords.Names) {
			foundName = GetName().Find(Name.ToString()) != INDEX_NONE ? true : false;
			if(foundName) break;
		}
		if(Gait.Keywords.bUseLocator) {
			for (auto Locator : Gait.Keywords.Locators) {
				foundLocator = GetName().Find(Locator.ToString()) != INDEX_NONE ? true : false;
				if(foundLocator) break;
			}
		}
			
		if((foundName && foundLocator) || (foundName && !Gait.Keywords.bUseLocator)) {
			GaitOffset = Gait.GaitOffset;
			break;
		}
	}
}

int ULimb::Solve_FABRIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations) {
	if(!Mesh || Iterations <= 0)
		return false;
	
	// ResetStates(Mesh);
	TArray<FVector> JointLocations;
	InitializeIK(Mesh, JointLocations);
	ResetRoll(Mesh);

	FTransform ToComponentSpace = Mesh->GetComponentTransform();
	FVector ComponentSpaceTarget = ToComponentSpace.InverseTransformPosition(FootPlan.Current.GetLocation());
	FVector StartJoint = Mesh->GetBoneLocationByName(Joints.Last().GetName(), EBoneSpaces::ComponentSpace);
	FVector StartToTarget = ComponentSpaceTarget - StartJoint;
	float dist = StartToTarget.Length();
	if(dist > MaxLength + Threshold) {
		FVector CurrentLocation = Mesh->GetBoneLocationByName(Joints.Last().GetName(), EBoneSpaces::ComponentSpace);
		FVector TgtFwd = (ComponentSpaceTarget - CurrentLocation).GetSafeNormal();
		Reach(Mesh, TgtFwd);
		return false; 
	}
	
	int k;
	for (k = 0; k < Iterations; k++) {
		FVector PreviousPosition = JointLocations[0];
		BackwardReach(Mesh, ComponentSpaceTarget, JointLocations);
		ForwardReach(Mesh, JointLocations);
		EvaluateAngles(Mesh, JointLocations);
		float sqrDist = GetEndToTargetOffset(ComponentSpaceTarget, Mesh, EBoneSpaces::ComponentSpace).SquaredLength();
		if(sqrDist < Threshold * Threshold) 
			return k;
	}
	return k;
}

void ULimb::InitializeIK(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations) {
	JointLocations.Reserve(Joints.Num());
	FTransform Transform = Mesh->GetComponentTransform();
	FVector OffsetDirection = 
		Transform.InverseTransformVector(FootPlan.Current.UpVector) + (1 * RestingTargetLocation.GetSafeNormal());
	OffsetDirection.Normalize();

	FVector Leaf = Mesh->GetBoneLocationByName(Joints.Last().GetName(), EBoneSpaces::WorldSpace);
	DrawDebugDirectionalArrow(GetWorld(), Leaf, Leaf + Transform.TransformVector(OffsetDirection * 100), 25, FColor::White);
	Reach(Mesh, OffsetDirection);
	
	for(int i = 0; i < Joints.Num(); ++i) {
		FVector Location = Mesh->GetBoneLocationByName(Joints[i].GetName(), EBoneSpaces::ComponentSpace);
		// FVector NewLocation = Location + OffsetDirection * 150;
		// DrawDebugLine(Mesh->GetWorld(), Transform.TransformPosition(Location), Transform.TransformPosition(NewLocation), FColor::Magenta);
		JointLocations.Add(Location);
	}
}

void ULimb::ResetRoll(UPoseableMeshComponent* Mesh) {

	for(int i = 1; i < Joints.Num(); i++) {
		FVector CurrentUp = Joints[i].GetState().GetForwardVector();
		FVector CurrentForward = Joints[i].GetState().GetRightVector();
		FVector RestUp = Joints[i].GetRestState().GetForwardVector();
		FVector NewUp = FVector::VectorPlaneProject(RestUp, CurrentForward);
		
		FQuat RollCorrection = FQuat::FindBetween(CurrentUp, NewUp);
		float angle = FMath::RadiansToDegrees(RollCorrection.GetAngle());
		Joints[i].SetState(RollCorrection * Joints[i].GetState());
		Mesh->SetBoneRotationByName(Joints[i].GetName(), Joints[i].GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

void ULimb::BackwardReach(UPoseableMeshComponent* Mesh, FVector ComponentSpaceTarget, TArray<FVector>& JointLocations) {
	//Mesh->SetBoneLocationByName(Joints[0]->GetName(), ComponentSpaceTarget, EBoneSpaces::ComponentSpace);
	JointLocations[0] = ComponentSpaceTarget;
	for(int i = 1; i < Joints.Num(); i++) {
		FVector CurrentLocation = GetJointLocation(i, JointLocations);
		FVector NextLocation = GetJointLocation(i-1, JointLocations);
		FVector Offset = NextLocation - CurrentLocation;
		float lengthRatio = 1.f - Joints[i].GetLength() / Offset.Length();
		FVector NewLocation = FMath::Lerp(CurrentLocation, NextLocation, lengthRatio);
		float dist = (NewLocation - CurrentLocation).Length();
		JointLocations[i] = NewLocation;
		// Mesh->SetBoneLocationByName(Joints[i]->GetName(), NewLocation, EBoneSpaces::ComponentSpace);
	}
}
void ULimb::ForwardReach(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations) {
	JointLocations.Last() = HipLocation;
	for(int i = Joints.Num() - 2; i >= 0; i--) {
		FVector CurrentLocation = GetJointLocation(i, JointLocations);
		FVector PreviousLocation = GetJointLocation(i+1, JointLocations);
		FVector Offset = CurrentLocation - PreviousLocation;
		float lengthRatio = Joints[i+1].GetLength() / Offset.Length();
		FVector NewLocation = FMath::Lerp(PreviousLocation, CurrentLocation, lengthRatio);
		float dist = (NewLocation - CurrentLocation).Length();
		JointLocations[i] = NewLocation;
		// Mesh->SetBoneLocationByName(Joints[i]->GetName(), NewLocation, EBoneSpaces::ComponentSpace);
		//Joints[i]->SetLocation(NewLocation);
	}
}

void ULimb::EvaluateAngles(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations) {
	for(int i = 1; i < Joints.Num(); i++) {
		FVector Fwd = -Joints[i].GetState().GetRightVector();
		FVector TgtFwd = (GetJointLocation(i-1, JointLocations) - GetJointLocation(i, JointLocations)).GetSafeNormal();
		// FVector Start = GetJointLocation(i, JointLocations);
		// FVector CurrentEnd = Start + Fwd * 20;
		// FVector TargetEnd = Start + TgtFwd * 20;
		// FTransform Transform = Mesh->GetComponentTransform();
		// DrawDebugLine(GetWorld(), Transform.TransformPosition(Start), Transform.TransformPosition(CurrentEnd),
		// 	FColor::Magenta, false, -1, -1);
		// DrawDebugLine(GetWorld(), Transform.TransformPosition(Start), Transform.TransformPosition(TargetEnd),
		// 	FColor::Orange, false, -1, -1);
		FQuat DeltaRotation = FQuat::FindBetween(Fwd, TgtFwd);
		// DeltaRotation = FQuat::Slerp(FQuat::Identity, DeltaRotation, 0.01);
		float angle = FMath::RadiansToDegrees(DeltaRotation.GetAngle());
		Joints[i].SetState(DeltaRotation * Joints[i].GetState());
		Mesh->SetBoneRotationByName(Joints[i].GetName(), Joints[i].GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

void ULimb::Reach(UPoseableMeshComponent* InMesh, FVector Direction) {
	for(int i = Joints.Num() - 1; i > 0; --i) {
		FVector Fwd = -Joints[i].GetState().GetRightVector();
		FQuat DeltaRotation = FQuat::FindBetween(Fwd, Direction);
		Joints[i].SetState(DeltaRotation * Joints[i].GetState());
		InMesh->SetBoneRotationByName(Joints[i].GetName(), Joints[i].GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

FVector ULimb::GetJointLocation(int Id, TArray<FVector> JointLocations) {
	
	// FVector Result = HipLocation;
	// for(int i = Id + 1; i < Joints.Num(); i++) 
	// 	Result += Joints[i]->GetState().GetForwardVector() * Joints[i]->GetLength();
	
	// return Mesh->GetBoneLocationByName(Joints[Id]->GetName(), EBoneSpaces::ComponentSpace);
	return JointLocations[Id];
}



void ULimb::DrawIK(UPoseableMeshComponent* Mesh, float Threshold) {
	auto Transform = Mesh->GetComponentTransform();
	DrawDebugBox(Mesh->GetWorld(), FootPlan.Current.GetLocation(), FVector::OneVector * 2 * Threshold, FColor::Green);
	DrawDebugDirectionalArrow(Mesh->GetWorld(), FootPlan.Current.GetLocation(), FootPlan.Current.GetLocation() + FootPlan.Current.UpVector * 35.f, 50.f,
		FColor::Purple, false, -1, 0, 1.5f);
	FVector End = Transform.TransformPosition(RestingTargetLocation);
	DrawDebugLine(Mesh->GetWorld(), FootPlan.Current.Location, End, FColor::Yellow);
	// DrawDebugSphere(GetWorld(), Transform.TransformPosition(HipLocation), MaxLength, 16, FColor::Yellow);
}

FVector ULimb::GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return Mesh->GetBoneLocationByName(Joints[0].GetName(), InSpace);
}

FVector ULimb::GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	if(Id < 0 || Id >= Joints.Num())
		return FVector::ZeroVector;
	return Mesh->GetBoneLocationByName(Joints[Id].GetName(), InSpace);
}

FVector ULimb::GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return  Target - GetEndLocation(Mesh, InSpace);
}

void ULimb::ResetStates(UPoseableMeshComponent* Mesh) {
	for(int i = Joints.Num() - 1; i > 0; --i) {
		Joints[i].SetState(Joints[i].GetRestState());
		Mesh->SetBoneRotationByName(Joints[i].GetName(), Joints[i].GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

bool ULimb::EvaluateTargetPosition(ULimb* InLimb, UPoseableMeshComponent* InMesh, float GaitCycleDuration, float TraceDistance, int Iterations,
									ECollisionChannel TraceChannel) {
	FHitResult Hit;
	FTransform Transform = InMesh->GetComponentTransform();
	FVector LinearVelocity = InMesh->GetOwner()->GetVelocity();
	FVector AngularVelocity = FVector::ZeroVector;
	FVector PointVelocity = LinearVelocity + AngularVelocity;
	FVector Offset = PointVelocity * GaitCycleDuration;
	FVector Direction = FootPlan.Current.UpVector;
	// FVector Start = InMesh->GetBoneLocationByName(InLimb->Joints[0].GetName(), EBoneSpaces::WorldSpace) + Offset;
	FVector Start = Transform.TransformPosition(RestingTargetLocation) + Offset;
	if(TraceAround(InLimb, InMesh, Start, -Direction, Iterations, TraceChannel, Start, Hit)) {
		FootPlan.Target = FIKEffector(Hit.ImpactPoint, Hit.Normal);
		return true;
	}
	return false;
}

FLimbSegment ULimb::MakeJoint(UPoseableMeshComponent* Mesh, FName BoneName, bool bIsEnd) {
	auto BoneTransform = Mesh->GetBoneTransformByName(BoneName, EBoneSpaces::ComponentSpace);
	if(bIsEnd) {
		auto MeshTransform = Mesh->GetComponentTransform();
		FVector Location = MeshTransform.TransformPosition(BoneTransform.GetLocation());
		RestingTargetLocation = BoneTransform.GetLocation();
		FootPlan.Current = FIKEffector(Location, MeshTransform.GetRotation());
		FootPlan.Start = FootPlan.Current;
	}
	auto LimbSegment = FLimbSegment(BoneName, BoneTransform.GetRotation());
	return LimbSegment;
}


bool ULimb::TraceFoot(ULimb* InLimb, UPoseableMeshComponent* Mesh, FVector InStart, FVector InDirection,
	ECollisionChannel InTraceChannel, FVector Rest, FHitResult& OutHit) {
	FTransform Transform = Mesh->GetComponentTransform();
	// FVector End = InStart + InDirection * InDistance;

	FVector WorldSpaceHipLoc = Transform.TransformPosition(InLimb->HipLocation);
	float radius = InLimb->MaxLength * 1.25;
	float firstFactor = InDirection.Dot(InStart - WorldSpaceHipLoc);
	float gradient = firstFactor * firstFactor - FVector::DistSquared(InStart, WorldSpaceHipLoc) + radius * radius;
	if(gradient <= 0)
		return false;
	float sqrtGrad = FMath::Sqrt(gradient);
	float distance1 = (-firstFactor + sqrtGrad) / 2;
	float distance2 = (-firstFactor - sqrtGrad) / 2;
	if(distance1 > distance2) {
		float temp = distance1;
		distance1 = distance2;
		distance2 = temp;
	}
	
	FVector NewStart = InStart + InDirection * distance1;
	FVector NewEnd = InStart + InDirection * distance2;
	
	TArray<AActor*> Ignore;
	Ignore.Add(Mesh->GetOwner());
	TArray<FHitResult> Hits;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		Mesh->GetWorld(),
		NewStart, NewEnd, 5.f,
		UEngineTypes::ConvertToTraceType(InTraceChannel),
		false,
		Ignore,
		EDrawDebugTrace::None,
		Hits, true,
		FLinearColor::Red, FLinearColor::Green,
		0.45);
	// if(bHit && OutHit.Normal.Dot(Mesh->GetUpVector()) < -.5f) {
	// 	
	// 	
	// 	OutHit.Normal = -OutHit.Normal;
	// 	OutHit.ImpactNormal = -OutHit.ImpactNormal;
	// }
	if(bHit) {
		OutHit.Distance = INFINITY;
		for (auto Hit : Hits) {
			if(Hit.bBlockingHit && FVector::DistSquared(Hit.Location, Rest) < FVector::DistSquared(OutHit.Location, Rest))
				OutHit = Hit;
		}
	}
	// if(bHit) {
	// 	bHit = UKismetSystemLibrary::SphereTraceSingle(
	// 		Mesh->GetWorld(),
	// 		OutHit.Location, InRestPosition, 10.f,
	// 		UEngineTypes::ConvertToTraceType(InTraceChannel),
	// 		false,
	// 		Ignore,
	// 		EDrawDebugTrace::None,
	// 		OutHit, true);
	// }
	return bHit;
}

bool ULimb::TraceAround(ULimb* InLimb, UPoseableMeshComponent* Mesh, FVector InStart, FVector InDirection,
					int Iterations, ECollisionChannel InTraceChannel, FVector Rest, FHitResult& OutHit) {

	FQuat DirAsRot = FQuat::FindBetweenNormals(FVector::DownVector, InDirection);
	for(int i = 0; i < Iterations; i++) {
		float theta = TWO_PI * i / UE_GOLDEN_RATIO;
		float phi = FMath::Acos(1 - (2.0 * i) / Iterations);
		FVector Direction = -FVector(
			FMath::Cos(theta) * FMath::Sin(phi),
			FMath::Sin(theta) * FMath::Sin(phi),
			FMath::Cos(phi)
			);
		Direction = DirAsRot * Direction;
		if(TraceFoot(InLimb, Mesh, InStart, Direction, InTraceChannel, Rest, OutHit)
			|| TraceFoot(InLimb, Mesh, InStart, -Mesh->GetUpVector(), InTraceChannel, Rest, OutHit))
			return true;
	}
	return false;
}
