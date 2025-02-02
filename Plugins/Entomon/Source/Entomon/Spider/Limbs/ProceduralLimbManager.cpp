#include "ProceduralLimbManager.h"

#include "LimbSegment.h"
#include "Components/PoseableMeshComponent.h"
#include "Entomon/Spider/MultiLeggedPawn.h"
#include "Entomon/Spider/GaitPreset.h"
#include "Entomon/Spider/MultiLeggedPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"

UProceduralLimbManager::UProceduralLimbManager() {
	//AutoDetectLimbs();
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UProceduralLimbManager::BeginPlay() {
	Super::BeginPlay();

	OwnerPawn = Cast<AMultiLeggedPawn>(GetOwner());
	if(OwnerPawn)
		Movement = OwnerPawn->GetMovementComponent();
}

void UProceduralLimbManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	float LastWalkCycleCounter;
	TickTimers(DeltaTime, LastWalkCycleCounter);
	TickLimbs(DeltaTime, LastWalkCycleCounter);
	// ApproachLimbAverageRotation(DeltaTime);
}

void UProceduralLimbManager::TickTimers(float DeltaTime, float& LastWalkCycleCounter) {
	LastWalkCycleCounter = WalkCycleCounter;
	float CycleDuration = GetLerpedWalkCycleDuration();
	WalkCycleCounter += DeltaTime / CycleDuration;
	if(WalkCycleCounter > 1.f)
		WalkCycleCounter -= FMath::Floor(WalkCycleCounter);
}

void UProceduralLimbManager::TickLimbs(float DeltaTime, float LastWalkCycleCounter) {
	BobbingOffset = 0;
	for(int i = 0; i < Limbs.Num(); ++i) {
		if((WalkCycleCounter > Limbs[i]->GaitOffset && LastWalkCycleCounter <= Limbs[i]->GaitOffset)
			|| (WalkCycleCounter >= Limbs[i]->GaitOffset && LastWalkCycleCounter > WalkCycleCounter))
		{
			Limbs[i]->TryMove(Mesh, OwnerPawn->GetBody(), GetLerpedWalkCycleDuration(),
				FootPlanningIterations, TraceChannel);
		}
		Limbs[i]->Tick(Mesh, DeltaTime, TraceChannel);
		Limbs[i]->UpdateIK(Mesh, IKThreshold, IKIterations, false);
		BobbingOffset+=FMath::Sin(PI * Limbs[i]->StepTimer)/Limbs.Num();
	}
}

void UProceduralLimbManager::ApplyGaitPreset(UGaitPreset* InGaitPreset) {
	WalkCycleDuration = InGaitPreset->WalkCycleDuration;
	MaxWalkCycleDuration = InGaitPreset->MaxWalkCycleDuration;
	GaitData = InGaitPreset;
	for(auto Limb : Limbs) {
		Limb->ApplyGaitPreset(InGaitPreset);
	}
}


void UProceduralLimbManager::AutoDetectLimbs(UPoseableMeshComponent* InMesh) {
	if(!InMesh) return;
	Mesh = InMesh;
	Limbs.Empty();

	TArray<FName> Bones;
	Mesh->GetBoneNames(Bones);
	//Mesh->GetSkinnedAsset()->GetSkeleton()->

	for(FName Bone : Bones) {
		auto boneAsString = Bone.ToString();
		auto templateName = EndEffectorsName.ToString();
		FString endName = "_end";
		int32 foundTip = boneAsString.Find(*templateName, ESearchCase::CaseSensitive);
		int32 foundEnd = boneAsString.Find(*endName, ESearchCase::CaseSensitive);
		
		if(foundTip != INDEX_NONE && foundEnd != INDEX_NONE) {
			auto LimbName = boneAsString;
			LimbName.RemoveAt(foundTip, templateName.Len());
			LimbName.RemoveFromEnd("_end");

			auto Limb = NewObject<ULimb>(this, ULimb::StaticClass(), *LimbName);
			Limb->Initialize(this, Mesh, Bone, HipJointsName);
			Limbs.Add(Limb);
		}
	}
	// for(auto& Leg : Limbs) {
	// 	for(int i = 1; i < Leg->Bones.Num(); ++i) {
	// 		FVector Displacement = Mesh->GetBoneLocationByName(Leg->Bones[i-1].Name, EBoneSpaces::ComponentSpace);
	// 			- Mesh->GetBoneLocationByName(Leg->Bones[i].Name, EBoneSpaces::ComponentSpace);
	// 		Leg->Bones[i].Length = Displacement.Length();
	// 	}
	// }
}

FVector UProceduralLimbManager::GetAverageLimbUpVector() const {
	if(Limbs.IsEmpty())
		return FVector::ZeroVector;

	
	FVector Centroid = FVector::ZeroVector;
	for (auto Limb : Limbs) {
		FVector Location = Limb->FootPlan.Current.GetLocation();
		Centroid += Location;
	}
	Centroid /= Limbs.Num();

	float xx=0,xy=0,xz=0,
		  yy=0,yz=0,zz=0;
	for (auto Limb : Limbs) {
		FVector r = Limb->FootPlan.Current.GetLocation() - Centroid;;
		
		xx += r.X * r.X; xy += r.X * r.Y;
		xz += r.X * r.Z; yy += r.Y * r.Y;
		yz += r.Y * r.Z; zz += r.Z * r.Z;
	}

	float DetX = yy*zz-yz*yz;
	float DetY = xx*zz-xz*xz;
	float DetZ = xx*yy-xy*xy;
	float DetMax=FMath::Max3(DetX,DetY,DetZ);
	if(DetMax<=0)
		return FVector::ZeroVector;
	FVector Direction;
	if(DetMax==DetX)
		Direction = FVector(DetX, xz*yz-xy*zz,xy*yz-xz*yy);
	else if(DetMax==DetY)
		Direction = FVector(xz*yz-xy*zz, DetY,xy*xz-yz*xx);
	else if(DetMax==DetZ)
		Direction = FVector(xy*yz-xz*yy, xy*xz-yz*xx, DetZ);

	return Direction.GetSafeNormal();
	
	// FVector Result = FVector::ZeroVector;
	// for(int i = 0; i < Limbs.Num(); i++) {
	// 	FTransform Transform = Mesh->GetComponentTransform();
	// 	FVector Current = Limbs[i]->FootPlan.Current.UpVector;
	// 	DrawDebugDirectionalArrow(Mesh->GetWorld(), Mesh->GetOwner()->GetActorLocation(), Mesh->GetOwner()->GetActorLocation() + Current * 200, 25, FColor::Red);
	// 	Result += Current;
	// 	//AvgRotation += RotationalOffset;
	// }
	// //AvgRotation.Normalize();
	// Result.Normalize();
	// return Result;
}

void UProceduralLimbManager::RegisterLimbLanding(ULimb* Limb) {
	// FVector FootLocation = Limb->FootPlan.Current.GetLocation();
	// FVector OwnerLocation = GetOwner()->GetActorLocation();
	// FVector Offset = FootLocation - OwnerLocation;
	// FVector UpVector = GetOwner()->GetActorUpVector();
	// FVector ImpulseAxis = UpVector.Cross(Offset).GetSafeNormal();
	// if(auto m = Cast<UMultiLeggedPawnMovement>(OwnerPawn->GetMovementComponent())) {
	// 	// m->AddAngularImpulse(ImpulseAxis * GaitData->BobbingImpulse, false);
	// 	m->Velocity += UpVector * GaitData->BobbingImpulse;
	// 	m->AngularVelocity += ImpulseAxis * GaitData->LandingAngularImpulse;
	// }
	
	// FVector FootDirection = Limb->FootPlan.Current.GetUpVector();
}

void UProceduralLimbManager::RegisterLimbTakeoff(ULimb* Limb) {
	// FVector FootLocation = Limb->FootPlan.Current.GetLocation();
	// FVector OwnerLocation = GetOwner()->GetActorLocation();
	// FVector Offset = FootLocation - OwnerLocation;
	// FVector UpVector = GetOwner()->GetActorUpVector();
	// FVector ImpulseAxis = UpVector.Cross(Offset).GetSafeNormal();
	// if(auto m = Cast<UMultiLeggedPawnMovement>(OwnerPawn->GetMovementComponent())) {
	// 	// m->AddAngularImpulse(ImpulseAxis * GaitData->BobbingImpulse, false);
	// 	m->Velocity += -UpVector * GaitData->BobbingImpulse;
	// 	m->AngularVelocity += -ImpulseAxis * GaitData->TakeoffAngularImpulse;
	// }
}

void UProceduralLimbManager::ApproachLimbAverageRotation(double DeltaTime) {
	FQuat CurrentRotation = GetOwner()->GetActorRotation().Quaternion();
	FVector CurrentUp = CurrentRotation.GetUpVector();
	FVector AverageUp = GetAverageLimbUpVector();
	FQuat DeltaRotation = FQuat::FindBetweenNormals(CurrentUp, AverageUp);
	FQuat NewRotation = FQuat::Slerp(CurrentRotation, DeltaRotation * CurrentRotation, DeltaTime * 5);
	GetOwner()->SetActorRotation(NewRotation);
	DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() + AverageUp * 150,100,
		FColor::Turquoise, false, -1, 0, 5);
}

float UProceduralLimbManager::GetLerpedWalkCycleDuration() {
	if(!Movement)
		return -1.f;
	float t = Movement->Velocity.Length() / Movement->GetMaxSpeed();
	return FMath::Lerp(MaxWalkCycleDuration, WalkCycleDuration, t);
}
