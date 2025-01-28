#include "MultiLeggedPawn.h"

#include "GaitPreset.h"
#include "MultiLeggedPawnMovement.h"
#include "Components/PoseableMeshComponent.h"
#include "Entomon/Navigation/NavVolume.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Limbs/ProceduralLimbManager.h"

AMultiLeggedPawn::AMultiLeggedPawn() {
	Body = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Body;
	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>("Mesh");
	Mesh->SetupAttachment(Body);
	LimbManager = CreateDefaultSubobject<UProceduralLimbManager>("Limb Manager");
	MovementComponent = CreateDefaultSubobject<UMultiLeggedPawnMovement>("Movement");
	
	//LimbManager->AutoDetectLimbs(Mesh);
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiLeggedPawn::MoveTo(FVector Location) {
	checkf(Navigation, TEXT("Nav Volume not found."));
	bIsMovementFinished = false;
	auto PathToTarget = Navigation->FindPath(Body->GetComponentLocation(), Location, PathPreference);
	bUseLookTarget = false;
	SetPath(PathToTarget);
}

void AMultiLeggedPawn::LookAt(FVector Location) {
	LookTarget = Location;
	bUseLookTarget = true;
}

void AMultiLeggedPawn::ResumeMove() {
	bUseLookTarget = false;
}

void AMultiLeggedPawn::SetPath(TArray<FNavNode> Nodes) {
	Path.Empty();
	for (int i = 0; i < Nodes.Num(); ++i) {
		FVector Next = i < Nodes.Num()-1 ? Nodes[i+1].Origin : FVector::ZeroVector;
		FPathNode Node = FPathNode(Nodes[i], Next);
		Path.Add(Node);
	}
	CurrentPathId = 0;
	if(bCorrectPath)
		CorrectPath();
	if(PathSmoothingType==EPathSmoothingType::Uniform)
		SmoothPath();
	if (bSimplify)
		SimplifyPath();
}

bool AMultiLeggedPawn::Move(double DeltaTime, int Target) {
	FVector ToTarget = Path[Target].Origin - GetActorLocation();
	float TargetDist = ToTarget.Length();
	FVector ToTargetNorm = ToTarget / TargetDist;

	float time = MovementComponent->MaxSpeed / MovementComponent->Deceleration;
	float brakeDistance = 0.5 * time * MovementComponent->MaxSpeed;

	float NodePlaneDistance = GetDistanceFromNodePlane(Target);
	if(NodePlaneDistance >= -brakeDistance && TargetDist < brakeDistance * 2)
		return true;
	
	float Dot = GetActorForwardVector().Dot(ToTargetNorm);
	float NormalizedFacingInverse = (1+Dot)*0.5;
	float InputModifier = (NormalizedFacingInverse + FacingBias)/(1.f+FacingBias);
	FVector Input = MovementComponent->bFaceTarget ? InputModifier * ToTargetNorm : ToTargetNorm;
	
	MovementComponent->AddInputVector(Input);
	
	return false;
}

FQuat AMultiLeggedPawn::GetTargetOrientation(int Target) {
	FVector UpVector = GetActorUpVector();
	
	FQuat RotationToNode = Target < Path.Num() ?
		FQuat::FindBetweenNormals(UpVector, Path[Target].Normal) : FQuat::FindBetweenNormals(UpVector, Path.Last().Normal);
	FQuat CombinedRotation = RotationToNode;
	if(Target<Path.Num()) {
		FVector ToTarget = Path[Target].Origin - GetActorLocation();
		float Distance = ToTarget.Length();
		FVector ToTargetNorm = ToTarget / Distance;
		FVector PlaneTarget = FVector::VectorPlaneProject(ToTargetNorm, UpVector).GetSafeNormal();
		FQuat RotationToFaceNode = FQuat::FindBetweenNormals(
			GetActorForwardVector(),
			PlaneTarget);
		CombinedRotation = MovementComponent->bFaceTarget ? RotationToFaceNode * CombinedRotation : CombinedRotation;
	}
	return CombinedRotation;
}

FQuat AMultiLeggedPawn::GetTargetOrientation(FVector Location) {
	FVector UpVector = GetActorUpVector();
	FVector ToTarget = Location - GetActorLocation();
	float Distance = ToTarget.Length();
	FVector ToTargetDir = ToTarget / Distance;
	
	FQuat RotationToNode = FQuat::FindBetweenNormals(GetActorForwardVector(), ToTargetDir);
	FQuat CombinedRotation = RotationToNode;
	FVector PlaneTarget = FVector::VectorPlaneProject(ToTargetDir, UpVector).GetSafeNormal();
	FQuat RotationToFaceNode = FQuat::FindBetweenNormals(
		GetActorForwardVector(),
		PlaneTarget);
	CombinedRotation = RotationToFaceNode;
	return CombinedRotation;
}

void AMultiLeggedPawn::FollowPath(double DeltaTime) {
	if (bUseLookTarget)
		MovementComponent->ApproachOrientation(GetTargetOrientation(LookTarget));
	else if(CurrentPathId < Path.Num()) {
		MovementComponent->ApproachOrientation(GetTargetOrientation(CurrentPathId));
		if(Move(DeltaTime, CurrentPathId)) {
			CurrentPathId++;
		}
	}
	else if(!Path.IsEmpty() && CurrentPathId >= Path.Num()-1) {
		MovementComponent->ApproachOrientation(GetTargetOrientation(CurrentPathId));
		bIsMovementFinished = true;
	}
}

FVector AMultiLeggedPawn::GetAngularVelocity() const {
	return MovementComponent->AngularVelocity;
}

void AMultiLeggedPawn::BeginPlay() {
	LimbManager->AutoDetectLimbs(Mesh);
	ApplyGaitPreset(GaitPreset);
	Navigation = Cast<ANavVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavVolume::StaticClass()));
	Super::BeginPlay();
}

void AMultiLeggedPawn::Tick(float DeltaTime) {

	// auto Whiskers = FibonacciTrace(Body->GetComponentLocation());
	// FVector WhiskerImpulse = GetClosestWhisker(Whiskers, true);
	FollowPath(DeltaTime);

	if(bDrawPath) {
		for(int i = 0; i < Path.Num(); ++i) {
			if(i < Path.Num()-1)
				DrawDebugLine(GetWorld(), Path[i].Origin, Path[i+1].Origin, FColor::Emerald);
			DrawDebugPoint(GetWorld(), Path[i].Origin, 5.f, FColor::Emerald);
		}
	}

	Super::Tick(DeltaTime);
}

void AMultiLeggedPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMultiLeggedPawn::ApplyGaitPreset(UGaitPreset* InGaitPreset) {
	if(!InGaitPreset)
		return;
	MovementComponent->MaxSpeed = InGaitPreset->MaxSpeed;
	MovementComponent->Acceleration = InGaitPreset->Acceleration;
	MovementComponent->Deceleration = InGaitPreset->Deceleration;
	LimbManager->ApplyGaitPreset(InGaitPreset);
}

bool AMultiLeggedPawn::Trace(FVector Start, FVector Direction, FHitResult& OutHit) {
	TArray<AActor*> Ignore;
	FVector End = Start + Direction * WhiskerRayDistance;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start, End,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		Ignore,
		EDrawDebugTrace::None,
		OutHit,
		true);

	return bHit;
}

FVector AMultiLeggedPawn::GetAlteredInput(FVector Input) {
	auto h = GetClosestWhisker(FibonacciTrace(GetActorLocation()), true);
	if(h.bBlockingHit) {
		FVector ToHit = h.Location - GetActorLocation();
		FVector ToHitDirection = ToHit.GetSafeNormal();
		float Dot = ToHitDirection.Dot(Input);
		if(Dot > 0) {
			FVector Offset = ToHitDirection * PreferredPersonalSpace - ToHit;
			return (Input + Dot * Offset / PreferredPersonalSpace).GetSafeNormal() * Input.Length();
		}
	}
	return Input;
}

TArray<FHitResult> AMultiLeggedPawn::FibonacciTrace(FVector Start) {
	TArray<FHitResult> Results;
	for(int i = 0; i < NumWhiskers; i++) {
		float theta = TWO_PI * i / UE_GOLDEN_RATIO;
		float phi = FMath::Acos(1 - (2.0 * i) / NumWhiskers);
		FVector Direction = -FVector(
			FMath::Cos(theta) * FMath::Sin(phi),
			FMath::Sin(theta) * FMath::Sin(phi),
			FMath::Cos(phi)
			);
		FHitResult NewHit;
		bool bHit = Trace(Start, Direction, NewHit);
		if(bHit) {
			NewHit.ImpactNormal = -Direction;
			NewHit.ImpactPoint = Start;
			Results.Add(NewHit);
		}
	}
	return Results;
}

FHitResult AMultiLeggedPawn::GetClosestWhisker(TArray<FHitResult> Hits, bool bDraw) {
	FVector Result = FVector::Zero();
	FHitResult Closest;
	Closest.Distance = INFINITY;
	FColor GoodColor = FColor::Green;
	FColor MediumColor = FColor::Yellow;
	FColor BadColor = FColor::Red;
	for (FHitResult Hit : Hits) {
		float Alpha = GetNormalizedOffsetFromPreference(Hit.Distance);
		if(Hit.Distance < Closest.Distance)
			Closest = Hit;
		// Result += Alpha * Hit.ImpactNormal / NumWhiskers;
		if(bDraw) {
			FColor Color;
			if(Alpha < 0.5f) {
				Alpha *= 2;
				float invA = 1 - Alpha;
				Color = FColor(
					MediumColor.R * Alpha + GoodColor.R * invA,
					MediumColor.G * Alpha + GoodColor.G * invA,
					MediumColor.B * Alpha + GoodColor.B * invA);
			}
			else {
				Alpha = 2 * Alpha - 1;
				float invA = 1 - Alpha;
				Color = FColor(
					BadColor.R * Alpha + MediumColor.R * invA,
					BadColor.G * Alpha + MediumColor.G * invA,
					BadColor.B * Alpha + MediumColor.B * invA);
			}
		
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.Location, Color);
			DrawDebugPoint(GetWorld(), Hit.Location, 5.f, Color);
		}
	}
	
	return Closest;
}

float AMultiLeggedPawn::GetNormalizedOffsetFromPreference(float Distance) {
	float TargetOffset = PreferredPersonalSpace - Distance;
	float High = Distance < PreferredPersonalSpace ? MinDistanceFromObstacle : MaxDistanceFromObstacle;
	return FMath::Clamp(TargetOffset / (PreferredPersonalSpace - High), 0, 1);
}

float AMultiLeggedPawn::GetDistanceFromNodePlane(int Node) {
	FVector Tangent = GetTangent(Node);
	FVector Location = Path[Node].Origin;
	float SignedDistance = (Tangent.X * Location.X + Tangent.Y * Location.Y + Tangent.Z * Location.Z);
	FPlane Plane = FPlane(Tangent, SignedDistance);
	
	float planeEq = Plane.PlaneDot(GetActorLocation());
	return planeEq;
}

FVector AMultiLeggedPawn::GetTangent(int AtId) {
	FVector Tangent = FVector::ZeroVector;
	if(AtId < Path.Num()-1)
		Tangent += (Path[AtId+1].Origin - Path[AtId].Origin).GetSafeNormal();
	else if(AtId > 0)
		Tangent += (Path[AtId].Origin - Path[AtId-1].Origin).GetSafeNormal();
	Tangent.Normalize();
	return Tangent;
}

void AMultiLeggedPawn::CorrectPath() {
	for(int i = 0; i < Path.Num(); i++) {
		Path[i].Origin += Path[i].Normal * (PreferredPersonalSpace - Path[i].SurfaceDistance);
	}
}

void AMultiLeggedPawn::SimplifyPath() {
	int k = 0;
	while(k < Path.Num()-1) {
		k++;
		FVector PreviousDirection = Path[k-1].Tangent;
		FVector CurrentDirection = Path[k].Tangent;
		
		float dot = CurrentDirection.Dot(PreviousDirection);
		float remappedDot = (1.f-dot);
		if(remappedDot < Simplify) {
			Path.RemoveAt(k);
			// if(k < Path.Num())
			// 	Path[k-1].Tangent = (Path[k].Origin - Path[k-1].Origin).GetSafeNormal();
			k--;
		}
	}
	for(int i = 0; i < Path.Num()-1; i++) {
		(Path[i+1].Origin - Path[i].Origin).ToDirectionAndLength(Path[i].Tangent, Path[i].NextNodeDistance);
	}
}

void AMultiLeggedPawn::SubdividePath() {
	if(Path.Num() < 2)
		return;
	for(int i = 1; i < Path.Num()-1; ++i) {
		FPathNode Previous = Path[i];
		FPathNode Next = Path[i+1];
		FPathNode Current;
		Current.Origin = 0.5 * (Previous.Origin + Next.Origin);
		Current.SurfaceDistance = 0.5 * (Previous.SurfaceDistance + Next.SurfaceDistance);
		Current.Normal = (Previous.Normal + Next.Normal).GetSafeNormal();
		Path.Insert(Current, i);
	}
}

void AMultiLeggedPawn::SmoothPath() {
	if(PathSmoothing==0)
		return;
	TArray<FPathNode> SmoothPath = SmoothMovingAverage(Path);
	if(bCurveCorrection) {
		for(int i = 1; i < SmoothPath.Num()-1; ++i) {
			FVector Curvature = (SmoothPath[i].Tangent - SmoothPath[i-1].Tangent) / SmoothPath[i].NextNodeDistance;
			if(Curvature!=FVector::ZeroVector)
				Curvature = Curvature / Curvature.SquaredLength();
			FVector OriginalCurvature = (Path[i].Tangent - Path[i-1].Tangent) / Path[i].NextNodeDistance;
			if(OriginalCurvature!=FVector::ZeroVector)
				OriginalCurvature = OriginalCurvature / OriginalCurvature.SquaredLength();
			
			FVector Shift = OriginalCurvature - Curvature;
			SmoothPath[i].Origin += Shift * 0.1;
		}
	}
	Path = SmoothPath;
}

TArray<FPathNode> AMultiLeggedPawn::SmoothMovingAverage(TArray<FPathNode> InPath) const {
	TArray<FPathNode> Result = InPath;
	for(int i = 1; i < InPath.Num()-1; ++i) {
		int min = FMath::Max(i-PathSmoothing, 0);
		int max = FMath::Min(i+PathSmoothing, InPath.Num());
		int kernel = 1+i-min;
		float invKernelSize = 1.f/kernel;
		FPathNode Node;
		for(int j = i; j >=min; --j) {
			Node.SurfaceDistance += InPath[j].SurfaceDistance * invKernelSize;
			Node.NextNodeDistance += InPath[j].NextNodeDistance * invKernelSize;
			Node.Origin += InPath[j].Origin * invKernelSize;
			Node.Normal += InPath[j].Normal * invKernelSize;
			Node.Tangent += InPath[j].Tangent * invKernelSize;
		}
		Node.Normal.Normalize();
		Node.Tangent.Normalize();
		Result[i] = Node;
	}
	return Result;
}

float AMultiLeggedPawn::GetNormalizedInterpolatorToNextNode() {
	if(CurrentPathId < 1)
		return 0;
	FVector ToTarget = GetActorLocation() - Path[CurrentPathId].Origin;
	FVector Projected = ToTarget.ProjectOnToNormal(Path[CurrentPathId].Tangent);
	float Dist = Projected.Length();
	float Result = 1.f - Dist / Path[CurrentPathId-1].NextNodeDistance;
	return FMath::Clamp(Result, 0, 1);
}

FVector AMultiLeggedPawn::GetBobbingImpulse() {
	if(!GaitPreset)
		return FVector::ZeroVector;
	FVector Result = GaitPreset->BobbingImpulse * LimbManager->GetAverageLimbUpVector().Cross(GetActorUpVector());
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + Result, 15, FColor::Red);
	return Result;
}

