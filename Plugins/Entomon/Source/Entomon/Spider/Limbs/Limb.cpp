#include "Limb.h"

#include "LimbSegment.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


void ULimb::UpdateIK(UPoseableMeshComponent* Mesh, bool bDraw) {
	auto it = CCDIK_BackwardBounce(Mesh, 1.f, 35, 0.2f);
	if(it > 0)
		GEngine->AddOnScreenDebugMessage(-1, 0.01, FColor::Orange, FString::Printf(TEXT("%i"), it));
	if (bDraw) {
		DrawIK(Mesh);
	}
}

bool ULimb::CCDIK_SmartBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance) {
	if(!Mesh)
		return false;
	
	auto Transform = Mesh->GetComponentTransform();
	FVector ComponentSpaceTarget = Transform.InverseTransformPosition(IKTarget);
	
	auto Space = EBoneSpaces::ComponentSpace;
	//Bones = RestPose;
	int k = 0;
	while(true) {
		for (int i = 1; i < Bones.Num(); ++i) {
			FVector RotationAxis = Bones[i]->GetState().GetRotationAxis();
			FVector RestRotationAxis = Bones[i]->GetRestState().GetRotationAxis();
			FQuat PoleCorrection = FQuat::FindBetween(RotationAxis, RestRotationAxis);
			float Angle = PoleCorrection.GetAngle();
			Bones[i]->GetState() = PoleCorrection * Bones[i]->GetState();
			ApplyBoneTransformation(Mesh);
			
			FVector CurrentLocation = GetCurrentLocation(i, Mesh, Space);
			FVector CurrentLocationWorld = Transform.TransformPosition(CurrentLocation);
			DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(RotationAxis) * 20.f, FColor::Red);
			DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(RestRotationAxis) * 20.f, FColor::Green);
			
			FVector ToEnd = GetCurrentLocation(0, Mesh, Space) - CurrentLocation;
			FVector ToTarget = ComponentSpaceTarget - CurrentLocation;
			FQuat IKRotation = GetRotatorBetween(ToEnd, ToTarget);
			Bones[i]->GetState() = IKRotation * Bones[i]->GetState();
			
			ApplyBoneTransformation(Mesh);
			
			float sqrDist = GetEndToTargetOffset(ComponentSpaceTarget, Mesh, Space).SquaredLength();
			if(sqrDist < Threshold * Threshold)
				return true;
			if(k / (Bones.Num() - 1) > Iterations)
				return false;
			auto newRotation = GetRotatorBetween(1, ComponentSpaceTarget, Mesh, Space);
			if(IKRotation.GetAngle() > 0.001)
				i = 0;
			k++;
			
		}
	}
}

int ULimb::CCDIK_BackwardBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance) {
	auto Transform = Mesh->GetComponentTransform();
	FVector ComponentSpaceTarget = Transform.InverseTransformPosition(IKTarget);
	auto Space = EBoneSpaces::ComponentSpace;
	
	// Bones = RestPose;
	int Max = FMath::Clamp(Bones.Num() - 1, 0, 3);
	int k = 0;
	while(k < Iterations) {
		for(int b = Max; b > 0; b--) {
			for(int i = 1; i <= b; i++) {
				FVector CurrentLocation = GetCurrentLocation(i, Mesh, Space);
				FVector ToEnd = GetCurrentLocation(0, Mesh, Space) - CurrentLocation;
				FVector ToTarget = ComponentSpaceTarget - CurrentLocation;
				FQuat IKRotation = GetRotatorBetween(ToEnd, ToTarget);
				IKRotation = FQuat::Slerp(FQuat::Identity, IKRotation, 1.2);
				// FVector Axis;
				// float Angle;
				// IKRotation.ToAxisAndAngle(Axis, Angle);
				// Angle = FMath::Clamp(Angle, 0.f, Angle-Tolerance);
				// IKRotation = FQuat::MakeFromRotationVector(Axis * Angle);
				
				// FVector RotationAxis = Bones[i]->GetState().GetRotationAxis();
				// FVector RestRotationAxis = Bones[i]->GetRestState().GetRotationAxis(); 
				// FQuat PoleCorrection = FQuat::FindBetween(RotationAxis, RestRotationAxis);
				// PoleCorrection = FQuat::Slerp(FQuat::Identity, PoleCorrection, 0.1);
				// IKRotation = PoleCorrection * IKRotation;
				
				Bones[i]->SetState(IKRotation * Bones[i]->GetState());

				Mesh->SetBoneRotationByName(Bones[i]->GetName(), Bones[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
				//ApplyBoneTransformation(Mesh);
				
				
				float sqrDist = GetEndToTargetOffset(ComponentSpaceTarget, Mesh, Space).SquaredLength();
				if(sqrDist < Threshold * Threshold) 
					return k;
			}
		}
		k++;
	}
	return k;
}

bool ULimb::JacobianIK_PseudoInverse(UPoseableMeshComponent* Mesh, float Threshold, int Iterations) {
	return true;
}

void ULimb::ApplyBoneTransformation(UPoseableMeshComponent* Mesh) {
	if(!Mesh) return;
	
	for (int i = 0; i < Bones.Num(); ++i) {
		Mesh->SetBoneRotationByName(Bones[i]->GetName(), Bones[i]->GetState().Rotator(), EBoneSpaces::ComponentSpace);
	}
}

void ULimb::DrawIK(UPoseableMeshComponent* Mesh) {
	DrawDebugSphere(Mesh->GetWorld(), IKTarget, 10.f, 4, FColor::Green, false, -1, -1);
	auto Transform = Mesh->GetComponentTransform();
	
	for (int i = 1; i < Bones.Num(); i++) {
		auto CurrentLocationWorld = GetCurrentLocation(i, Mesh, EBoneSpaces::WorldSpace);
		DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(Bones[i]->GetState().GetRotationAxis()) * 20.f, FColor::Red);
		DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(Bones[i]->GetRestState().GetRotationAxis()) * 20.f, FColor::Green);
	}
}

FVector ULimb::GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return Mesh->GetBoneLocationByName(Bones[0]->GetName(), InSpace);
}

FVector ULimb::GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	if(Id < 0 || Id >= Bones.Num())
		return FVector::ZeroVector;
	return Mesh->GetBoneLocationByName(Bones[Id]->GetName(), InSpace);
}

FVector ULimb::GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return  Target - GetEndLocation(Mesh, InSpace);
}

bool ULimb::PrefersTargetRelocation(UPoseableMeshComponent* Mesh, float MaxDistance, FVector& Displacement) {
	if(!Mesh)
		return false;
	FVector CurrentEndEffectorPosition = Mesh->GetBoneLocationByName(Bones[0]->GetName(), EBoneSpaces::ComponentSpace);
	Displacement = CurrentEndEffectorPosition - RestingTargetLocation;
	float Distance = Displacement.Length();
	if(Distance > MaxDistance) {
		Displacement = Displacement.GetClampedToMaxSize(MaxDistance);
		return true;
	}
	return false;
}

bool ULimb::Initialize(UPoseableMeshComponent* Mesh, FName EndEffectorName, FName HipNameToSearchFor) {
	if(!Mesh) return false;
	
	Bones.Add(MakeNode(Mesh, EndEffectorName, HipNameToSearchFor, true));
	FName CurrentBone = EndEffectorName;
	while(true) {
		CurrentBone = Mesh->GetParentBone(CurrentBone);
		if(CurrentBone.IsNone())
			return false;
		auto boneAsString = CurrentBone.ToString();
		auto templateName = HipNameToSearchFor.ToString();
		auto s = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		Bones.Add(MakeNode(Mesh, CurrentBone, HipNameToSearchFor));
		if(s != INDEX_NONE)
			return true;
	}
}

ULimbSegment* ULimb::MakeNode(UPoseableMeshComponent* Mesh, FName BoneName, FName HipNameToSearchFor, bool bIsRoot) {
	auto Transform = Mesh->GetBoneTransformByName(BoneName, EBoneSpaces::ComponentSpace);
	auto LimbSegment = NewObject<ULimbSegment>(this, ULimbSegment::StaticClass(), BoneName);
	LimbSegment->Initialize(BoneName, Transform.GetRotation());
	if(bIsRoot) {
		FVector Location = Mesh->GetComponentTransform().TransformPosition(Transform.GetLocation());
		RestingTargetLocation = Location;
		IKTarget = Mesh->GetComponentTransform().TransformPosition(Transform.GetLocation());
		//IKTarget->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform);
		//auto IKTargetName = FString::Printf(TEXT("IK_"));
		//IKTargetName.AppendChars(*BoneName.ToString(), BoneName.GetStringLength());
		//IKTarget->Rename(*IKTargetName);
		//Mesh->GetOwner()->AddInstanceComponent(IKTarget);
	}
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
