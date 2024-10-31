#include "Limb.h"

#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

void FLeg::Update(UPoseableMeshComponent* Mesh, bool bDraw) {
	CCDIK_SmartBounce(Mesh, 1.f, 35, 0.1f);
	if (bDraw) {
		DrawIK(Mesh);
	}
}

bool FLeg::CCDIK_SmartBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance) {
	if(!Mesh || !IKTarget)
		return false;
	
	auto Transform = Mesh->GetOwner()->GetTransform();
	FVector ComponentSpaceTarget = Transform.InverseTransformPosition(IKTarget->GetComponentLocation());
	
	auto Space = EBoneSpaces::ComponentSpace;
	//Bones = RestPose;
	int k = 0;
	while(true) {
		for (int i = 1; i < Bones.Num(); ++i) {
			FVector RotationAxis = Bones[i].State.Quaternion().GetRotationAxis();
			FVector RestRotationAxis = RestPose[i].State.Quaternion().GetRotationAxis();
			FQuat PoleCorrection = FQuat::FindBetween(RotationAxis, RestRotationAxis);
			float Angle = PoleCorrection.GetAngle();
			Bones[i].State += PoleCorrection.Rotator();
			ApplyBoneTransformation(Mesh);
			
			FVector CurrentLocation = GetCurrentLocation(i, Mesh, Space);
			FVector CurrentLocationWorld = Transform.TransformPosition(CurrentLocation);
			DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(RotationAxis) * 20.f, FColor::Red);
			DrawDebugLine(Mesh->GetWorld(), CurrentLocationWorld, CurrentLocationWorld + Transform.TransformVector(RestRotationAxis) * 20.f, FColor::Green);
			
			FVector ToEnd = GetCurrentLocation(0, Mesh, Space) - CurrentLocation;
			FVector ToTarget = ComponentSpaceTarget - CurrentLocation;
			FQuat IKRotation = GetRotatorBetween(ToEnd, ToTarget);
			Bones[i].State += IKRotation.Rotator();
			
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

bool FLeg::CCDIK_BackwardBounce(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, float Tolerance) {

	FVector ComponentSpaceTarget = Mesh->GetOwner()->GetTransform().InverseTransformPosition(IKTarget->GetComponentLocation());
	auto Space = EBoneSpaces::ComponentSpace;
	
	Bones = RestPose;
	int k = 0;
	while(k < Threshold) {
		for(int b = Bones.Num() - 1; b > 0; b--) {
			for(int i = 1; i < b; i++) {
				
				FVector CurrentLocation = GetCurrentLocation(i, Mesh, Space);
				FVector ToEnd = GetCurrentLocation(0, Mesh, Space) - CurrentLocation;
				FVector ToTarget = ComponentSpaceTarget - CurrentLocation;
				FQuat IKRotation = GetRotatorBetween(ToEnd, ToTarget);
				Bones[i].State += IKRotation.Rotator();

				float Angle = FMath::RadiansToDegrees(IKRotation.GetAngle());
				ApplyBoneTransformation(Mesh);

				float sqrDist = GetEndToTargetOffset(ComponentSpaceTarget, Mesh, Space).SquaredLength();
				if(sqrDist < Threshold * Threshold)
					return true;
			}
		}
		k++;
	}
	return false;
}

void FLeg::ApplyBoneTransformation(UPoseableMeshComponent* Mesh) {
	if(!Mesh || !IKTarget) return;
	
	for (int i = 0; i < Bones.Num(); ++i) {
		Mesh->SetBoneRotationByName(Bones[i].Name, Bones[i].State, EBoneSpaces::ComponentSpace);
	}
}

void FLeg::DrawIK(UPoseableMeshComponent* Mesh) {
	DrawDebugSphere(Mesh->GetWorld(), IKTarget->GetComponentLocation(), 10.f, 4, FColor::Green, false, -1, -1);
}

FVector FLeg::GetEndLocation(UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return Mesh->GetBoneLocationByName(Bones[0].Name, InSpace);
}

FVector FLeg::GetCurrentLocation(int Id, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	if(Id < 0 || Id >= Bones.Num())
		return FVector::ZeroVector;
	return Mesh->GetBoneLocationByName(Bones[Id].Name, InSpace);
}

FVector FLeg::GetEndToTargetOffset(FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	return  Target - GetEndLocation(Mesh, InSpace);
}

FQuat FLeg::GetRotatorBetween(FVector ToEnd, FVector ToTarget) {
	return FQuat::FindBetweenVectors(ToEnd, ToTarget);
}

FQuat FLeg::GetRotatorBetween(int Id, FVector Target, UPoseableMeshComponent* Mesh, EBoneSpaces::Type InSpace) {
	FVector CurrentLocation = GetCurrentLocation(Id, Mesh, InSpace);
	FVector ToEnd = GetEndLocation(Mesh, InSpace) - CurrentLocation;
	FVector ToTarget = Target - CurrentLocation;
	return GetRotatorBetween(ToEnd, ToTarget);
}
