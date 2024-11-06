#include "Limb.h"

#include "LimbSegment.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


void ULimb::UpdateIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw) {
	auto it = Solve_FABRIK(Mesh, Threshold, Iterations);
	//if(it > 0)
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("%i"), it));
	if (bDraw) {
		DrawIK(Mesh, Threshold);
	}
}

int ULimb::Solve_CCDIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations) {
	auto Transform = Mesh->GetComponentTransform();
	FVector ComponentSpaceTarget = Transform.InverseTransformPosition(IKTarget.GetLocation());
	auto Space = EBoneSpaces::ComponentSpace;
	
	

	int Max = FMath::Clamp(Joints.Num() - 1, 0, 3);
	int k = 0;
	while(k < Iterations) {
		for(int b = Joints.Num() - 1; b > 0; b--) {
			for(int i = 1; i <= b; i++) {
				FVector CurrentLocation = GetCurrentLocation(i, Mesh, Space);
				FVector ToEnd = GetCurrentLocation(0, Mesh, Space) - CurrentLocation;
				FVector ToTarget = ComponentSpaceTarget - CurrentLocation;
				FQuat IKRotation = GetRotatorBetween(ToEnd, ToTarget);
				//IKRotation = FQuat::Slerp(FQuat::Identity, IKRotation, 1.2);
				
				Joints[i]->SetState(IKRotation * Joints[i]->GetState());
				Mesh->SetBoneRotationByName(Joints[i]->GetName(), Joints[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);

				float sqrDist = GetEndToTargetOffset(ComponentSpaceTarget, Mesh, Space).SquaredLength();
				if(sqrDist < Threshold * Threshold) 
					return k;
			}
		}
		k++;
	}
	return k;
}

int ULimb::Solve_FABRIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations) {
	if(!Mesh)
		return false;
	
	// ResetStates(Mesh);

	FTransform ToComponentSpace = Mesh->GetComponentTransform();
	FVector ComponentSpaceTarget = ToComponentSpace.InverseTransformPosition(IKTarget.GetLocation());
	FVector StartJoint = Mesh->GetBoneLocationByName(Joints.Last()->GetName(), EBoneSpaces::ComponentSpace);
	FVector StartToTarget = ComponentSpaceTarget - StartJoint;
	float dist = StartToTarget.Length();
	if(dist > MaxLength + Threshold) {
		for(int i = Joints.Num() - 1; i > 0; --i) {
			FVector Fwd = -Joints[i]->GetState().GetRightVector();
			FVector CurrentLocation = Mesh->GetBoneLocationByName(Joints[i]->GetName(), EBoneSpaces::ComponentSpace);
			FVector TgtFwd = (ComponentSpaceTarget - CurrentLocation).GetSafeNormal();
			FQuat DeltaRotation = FQuat::FindBetween(Fwd, TgtFwd);
			Joints[i]->SetState(DeltaRotation * Joints[i]->GetState());
			Mesh->SetBoneRotationByName(Joints[i]->GetName(), Joints[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
		}
		return false; 
	}

	
	TArray<FVector> JointLocations;
	InitializeIK(Mesh, JointLocations);
	//CorrectPoles(Mesh);
	
	int k;
	for (k = 0; k < Iterations; k++) {
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
	for(int i = Joints.Num() - 1; i >= 0; --i) {
		FVector Location = Mesh->GetBoneLocationByName(Joints[i]->GetName(), EBoneSpaces::ComponentSpace);
		Location += IKTarget.GetRotation().GetForwardVector() * 100;
		JointLocations.Add(Location);
	}
}

void ULimb::BackwardReach(UPoseableMeshComponent* Mesh, FVector ComponentSpaceTarget, TArray<FVector>& JointLocations) {
	//Mesh->SetBoneLocationByName(Joints[0]->GetName(), ComponentSpaceTarget, EBoneSpaces::ComponentSpace);
	JointLocations[0] = ComponentSpaceTarget;
	for(int i = 1; i < Joints.Num(); i++) {
		FVector CurrentLocation = GetJointLocation(i, JointLocations);
		FVector NextLocation = GetJointLocation(i-1, JointLocations);
		FVector Offset = NextLocation - CurrentLocation;
		float lengthRatio = 1.f - Joints[i]->GetLength() / Offset.Length();
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
		float lengthRatio = Joints[i+1]->GetLength() / Offset.Length();
		FVector NewLocation = FMath::Lerp(PreviousLocation, CurrentLocation, lengthRatio);
		float dist = (NewLocation - CurrentLocation).Length();
		JointLocations[i] = NewLocation;
		// Mesh->SetBoneLocationByName(Joints[i]->GetName(), NewLocation, EBoneSpaces::ComponentSpace);
		//Joints[i]->SetLocation(NewLocation);
	}
}

void ULimb::EvaluateAngles(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations) {
	for(int i = 1; i < Joints.Num(); i++) {
		FVector Fwd = -Joints[i]->GetState().GetRightVector();
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
		Joints[i]->SetState(DeltaRotation * Joints[i]->GetState());
		Mesh->SetBoneRotationByName(Joints[i]->GetName(), Joints[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

FVector ULimb::GetJointLocation(int Id, TArray<FVector> JointLocations) {
	
	// FVector Result = HipLocation;
	// for(int i = Id + 1; i < Joints.Num(); i++) 
	// 	Result += Joints[i]->GetState().GetForwardVector() * Joints[i]->GetLength();
	
	// return Mesh->GetBoneLocationByName(Joints[Id]->GetName(), EBoneSpaces::ComponentSpace);
	return JointLocations[Id];
}


FVector ULimb::GetPoleAxisVector(UPoseableMeshComponent* Mesh) {
	FVector EndEffectorLocation = Mesh->GetBoneLocationByName(Joints[0]->GetName(), EBoneSpaces::ComponentSpace);
	FVector StartEffectorLocation = Mesh->GetBoneLocationByName(Joints[Joints.Num()-1]->GetName(), EBoneSpaces::ComponentSpace);
	return EndEffectorLocation - StartEffectorLocation;
}
FVector ULimb::GetPoleAxisVector_World(UPoseableMeshComponent* Mesh) {
	auto Transform = Mesh->GetComponentTransform();
	return Transform.TransformVector(GetPoleAxisVector(Mesh));
}

FVector ULimb::GetPoleNormal(UPoseableMeshComponent* Mesh, int Id) {
	auto Axis = (GetPoleAxisVector_World(Mesh) - IKTarget.GetLocation()).GetSafeNormal();
	auto PoleDir = IKTarget.GetRotation().GetUpVector();
	FVector Result = PoleDir.Cross(Axis);
	// if(Result.Z < 0)
	// 	Result = -Result;
	// float Dot1 = Result.Dot(PoleDir);
	// float Dot2 = Result.Dot(Axis);
	// auto BoneTransform = Mesh->GetBoneTransformByName(Bones[Id]->GetName(), EBoneSpaces::WorldSpace);
	// Result = (Result - Result.ProjectOnTo(Axis)).GetSafeNormal();

	FVector RotationAxis = Joints[Id]->GetRestState().GetRotationAxis();
	if(RotationAxis.Dot(Result) < RotationAxis.Dot(-Result))
		Result = -Result;
	
	return Result.GetSafeNormal();
}

void ULimb::CorrectPoles(UPoseableMeshComponent* Mesh, TArray<FVector>& JointLocations) {
	//if(Bones.Num() < 3) return;
	for(int i = Joints.Num() - 1; i > 0; i--) {
		// FVector RotationAxis = Bones[i]->GetState().GetRotationAxis();
		// FVector TargetRotationAxis = IKTarget.GetRotation().GetUpVector();
		// FQuat PoleCorrection = FQuat::FindBetween(RotationAxis, TargetRotationAxis);
		// // PoleCorrection = FQuat::Slerp(FQuat::Identity, PoleCorrection, 0.1);
		//
		// Bones[i]->SetState(PoleCorrection * Bones[i]->GetState());
		// Mesh->SetBoneRotationByName(Bones[i]->GetName(), Bones[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
		// Joints[i]->SetState(Joints[i]->GetRestState());
		// Mesh->SetBoneRotationByName(Joints[i]->GetName(), Joints[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);

		// JointLocations[i] += FVector::UpVector * 100;
	}
}

void ULimb::ApplyBoneTransformation(UPoseableMeshComponent* Mesh) {
	if(!Mesh) return;
	
	for (int i = 0; i < Joints.Num(); ++i) {
		Mesh->SetBoneRotationByName(Joints[i]->GetName(), Joints[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

void ULimb::DrawIK(UPoseableMeshComponent* Mesh, float Threshold) {
	auto Transform = Mesh->GetComponentTransform();
	FVector EndOffset = GetPoleAxisVector_World(Mesh);
	FVector Start = GetCurrentLocation(Joints.Num()-1, Mesh, EBoneSpaces::WorldSpace);
	DrawDebugLine(Mesh->GetWorld(), Start, Start + EndOffset, FColor::Blue);
	// DrawDebugSphere(Mesh->GetWorld(), Start + EndOffset + GetPoleNormal(Mesh, 0) * 100, 10.f, 4, FColor::Orange);
	DrawDebugBox(Mesh->GetWorld(), IKTarget.GetLocation(), FVector::OneVector * 2 * Threshold, FColor::Green);
	DrawDebugDirectionalArrow(Mesh->GetWorld(), IKTarget.GetLocation(), IKTarget.GetLocation() + IKTarget.GetRotation().GetForwardVector() * 35.f, 50.f,
		FColor::Purple, false, -1, 0, 1.5f);
	for (int i = 1; i < Joints.Num(); i++) {
		auto CurrentLocationWorld = GetCurrentLocation(i, Mesh, EBoneSpaces::WorldSpace);
		DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(Joints[i]->GetState().GetRotationAxis()) * 20.f, FColor::Red);
		DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(Joints[i]->GetRestState().GetRotationAxis()) * 20.f, FColor::Green);
		//DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + GetPoleNormal(Mesh, i) * 20.f, FColor::Turquoise);
		//DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + (IKTarget.GetRotation().GetUpVector()).GetSafeNormal() * 20.f, FColor::Silver);
	}
}

FVector ULimb::GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return Mesh->GetBoneLocationByName(Joints[0]->GetName(), InSpace);
}

FVector ULimb::GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	if(Id < 0 || Id >= Joints.Num())
		return FVector::ZeroVector;
	return Mesh->GetBoneLocationByName(Joints[Id]->GetName(), InSpace);
}

FVector ULimb::GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return  Target - GetEndLocation(Mesh, InSpace);
}

bool ULimb::PrefersTargetRelocation(UPoseableMeshComponent* Mesh, float MaxDistance, FVector& Displacement) {
	if(!Mesh)
		return false;
	FVector CurrentEndEffectorPosition = Mesh->GetBoneLocationByName(Joints[0]->GetName(), EBoneSpaces::ComponentSpace);
	Displacement = CurrentEndEffectorPosition - RestingTargetLocation;
	float Distance = Displacement.Length();
	if(Distance > MaxDistance) {
		Displacement = Displacement.GetClampedToMaxSize(MaxDistance);
		return true;
	}
	return false;
}

void ULimb::ResetStates(UPoseableMeshComponent* Mesh) {
	for(int i = Joints.Num() - 1; i > 0; --i) {
		Joints[i]->SetState(Joints[i]->GetRestState());
		Mesh->SetBoneRotationByName(Joints[i]->GetName(), Joints[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

bool ULimb::Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor) {
	if(!Mesh) return false;
	
	Joints.Add(MakeNode(Mesh, EndEffectorName, true));
	FName CurrentBone = EndEffectorName;
	while(true) {
		CurrentBone = Mesh->GetParentBone(CurrentBone);
		if(CurrentBone.IsNone())
			return false;
		auto boneAsString = CurrentBone.ToString();
		auto templateName = HipNameToSearchFor.ToString();
		auto s = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		Joints.Add(MakeNode(Mesh, CurrentBone));
		if(s != INDEX_NONE)
			break;
	}
	if(Joints.Num() > 1) {
		FQuat Rot = Joints[1]->GetState();
		IKTarget.SetRotation(FQuat::MakeFromRotationVector(Rot.GetUpVector() * PI / 2) * Rot);
	}
	for(int i = 1; i < Joints.Num(); i++) {
		FVector CurrentLocation = Mesh->GetBoneLocationByName(Joints[i]->GetName(), EBoneSpaces::ComponentSpace);
		FVector NextLocation = Mesh->GetBoneLocationByName(Joints[i-1]->GetName(), EBoneSpaces::ComponentSpace);
		FVector Offset = NextLocation - CurrentLocation;
		float Length = Offset.Length();
		Joints[i]->SetLength(Length);
		MaxLength += Length;
	}
	HipLocation = Mesh->GetBoneLocationByName(Joints.Last()->GetName(), EBoneSpaces::ComponentSpace);
	// Pole
	
	
	return true;
}

ULimbSegment* ULimb::MakeNode(UPoseableMeshComponent* Mesh, FName BoneName, bool bIsRoot) {
	auto Transform = Mesh->GetBoneTransformByName(BoneName, EBoneSpaces::ComponentSpace);
	auto LimbSegment = NewObject<ULimbSegment>(this, ULimbSegment::StaticClass(), BoneName);
	if(bIsRoot) {
		FVector Location = Mesh->GetComponentTransform().TransformPosition(Transform.GetLocation());
		RestingTargetLocation = Location;
		IKTarget.SetLocation(Mesh->GetComponentTransform().TransformPosition(Transform.GetLocation()));
	}
	LimbSegment->Initialize(BoneName, Transform.GetRotation());
	return LimbSegment;
}

FQuat ULimb::GetRotatorBetween(FVector ToEnd, FVector ToTarget) {
	return FQuat::FindBetweenVectors(ToEnd, ToTarget);
}

FQuat ULimb::GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	FVector CurrentLocation = GetCurrentLocation(Id, Mesh, InSpace);
	FVector ToEnd = GetEndLocation(Mesh, InSpace) - CurrentLocation;
	FVector ToTarget = Target - CurrentLocation;
	return GetRotatorBetween(ToEnd, ToTarget);
}
