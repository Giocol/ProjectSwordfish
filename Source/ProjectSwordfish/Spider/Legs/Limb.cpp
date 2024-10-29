#include "Limb.h"

#include "Components/PoseableMeshComponent.h"

void FLeg::Update(UPoseableMeshComponent* Mesh) {
	CCDIK(Mesh, 1.f, 10, true);
}

bool FLeg::CCDIK(UPoseableMeshComponent* Mesh, float Threshold, int Iterations, bool bDraw) {
	if(!Mesh || !IKTarget)
		return false;
	
	FTransform MeshTransform = Mesh->GetComponentTransform();
	FVector ComponentSpaceTarget = Mesh->GetComponentTransform().InverseTransformPosition(IKTarget->GetComponentLocation());
	auto Space = EBoneSpaces::ComponentSpace;

	auto StartRotation = Bones;
	
	int k = 0;
	while(true) {
		for (int i = 1; i < Bones.Num(); ++i) {
			
			FVector CurrentLocation = GetCurrentLocation(i, Mesh, Space);
			FVector ToEnd = GetCurrentLocation(0, Mesh, Space) - CurrentLocation;
			FVector ToTarget = ComponentSpaceTarget - CurrentLocation;
			float CosAngle = ToEnd.GetSafeNormal().Dot(ToTarget.GetSafeNormal());
			if(CosAngle == 0)
				continue;
			float Angle = FMath::Acos(CosAngle);
			FVector Axis = ToEnd.GetSafeNormal().Cross(ToTarget.GetSafeNormal()).GetSafeNormal();
			
			Bones[i].State += FQuat::MakeFromRotationVector(Axis * Angle).Rotator();
			ApplyBoneTransformation(Mesh, bDraw);

			CosAngle = ToEnd.GetSafeNormal().Dot(ToTarget.GetSafeNormal());
			Angle = CosAngle != 0.f ? FMath::Acos(CosAngle) : 0.f;
			
			if(GetEndToTargetOffset(ComponentSpaceTarget, Mesh, Space).Length() < Threshold)
				return true;
			if(k / Bones.Num() > Iterations) {
				//Bones = StartRotation;
				return false;
			}
			if(Angle > 0.001) {
				i = 0;
			}
			k++;
		}
	}
}

void FLeg::ApplyBoneTransformation(UPoseableMeshComponent* Mesh, bool bDraw) {
	if(!Mesh || !IKTarget) return;
	
	for (int i = 0; i < Bones.Num(); ++i) {
		Mesh->SetBoneRotationByName(Bones[i].Name, Bones[i].State, EBoneSpaces::ComponentSpace);
			
		//if(bDraw && i < Bones.Num() - 1) {
		//	FTransform T = Mesh->GetOwner()->GetActorTransform();
		//	DrawDebugLine(Mesh->GetWorld(), GetCurrentLocation(i, Mesh, EBoneSpaces::WorldSpace), GetCurrentLocation(i + 1, Mesh, EBoneSpaces::WorldSpace), FColor::Blue, false, -1, -1);
		//		
		//} 
	}
	if (bDraw) {
		DrawDebugSphere(Mesh->GetWorld(), IKTarget->GetComponentLocation(), 10.f, 4, FColor::Green, false, -1, -1);
	}
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
