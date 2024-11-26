#include "MultiLeggedPawn.h"

#include "GaitPreset.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Limbs/ProceduralLimbManager.h"

AMultiLeggedPawn::AMultiLeggedPawn() {
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	Body = CreateDefaultSubobject<USceneComponent>("Body");
	Body->SetupAttachment(Root);
	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
	LimbManager = CreateDefaultSubobject<UProceduralLimbManager>("Limb Manager");
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	
	//LimbManager->AutoDetectLimbs(Mesh);
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiLeggedPawn::SetPath(TArray<FNavNode> Nodes) {
	Path.Empty();
	for (int i = 0; i < Nodes.Num(); ++i) {
		FVector Next = i < Nodes.Num()-1 ? Nodes[i+1].Origin : FVector::ZeroVector;
		FPathNode Node = FPathNode(Nodes[i], Next);
		Path.Add(Node);
	}
	CurrentPathId = 0;
	CorrectPath();
	if(PathSmoothingType==EPathSmoothingType::Uniform)
		SmoothPath();
	SimplifyPath();
}

bool AMultiLeggedPawn::Move(double DeltaTime, int Target) {
	FVector ToTarget = Path[Target].Origin - Body->GetComponentLocation();
	FVector ToTargetNorm = ToTarget.GetSafeNormal();

	
	float time = MovementComponent->MaxSpeed / MovementComponent->Deceleration;
	float brakeDistance = 0.5 * time * MovementComponent->MaxSpeed;

	float TargetDist = ToTarget.Length();
	float NodeDistance = GetDistanceFromNodePlane(Target);
	if(NodeDistance >= -brakeDistance)
		return true;

	// float dist = SurfaceOffset.Length();
	// float alpha = GetNormalizedOffsetFromPreference(dist);
	FVector Input = ToTargetNorm;
	// FVector FromPrevious = Target > 0 && Target < Path.Num() ? Path[Target].Origin - Path[Target-1].Origin : FVector::ZeroVector;
	// FVector ToNext = Target >= 0 && Target < Path.Num() - 1 ? Path[Target+1].Origin - Path[Target].Origin : FVector::ZeroVector;
	// if(FromPrevious != FVector::ZeroVector) {
	// 	float Len;
	// 	FVector Dir;
	// 	FromPrevious.ToDirectionAndLength(Dir, Len);
	// 	float Alpha = FMath::Clamp(TargetDist / Len, 0, 1);
	// 	Input = FMath::Lerp(Input, Dir, Alpha);
	// }
	// if(ToNext != FVector::ZeroVector) {
	// 	float Len;
	// 	FVector Dir;
	// 	ToNext.ToDirectionAndLength(Dir, Len);
	// 	float Alpha = FMath::Clamp(TargetDist / Len, 0, 1);
	// 	Input = FMath::Lerp(Input, Dir, Alpha);
	// }
	
	// float InputModifier = FMath::Pow(NormalizedFacingInverse, (1-FacingBias) / FacingBias);
	MovementComponent->AddInputVector(Input);
	
	return false;
}

void AMultiLeggedPawn::Rotate(double DeltaTime, int Target) {
	FVector ToTarget = Path[Target].Origin - Body->GetComponentLocation();
	FVector ToTargetNorm = ToTarget.GetSafeNormal();

	FQuat DeltaQuat = FQuat::FindBetweenNormals(GetActorUpVector(), Path[Target].Normal);
	FVector PlaneTarget = FVector::VectorPlaneProject(ToTargetNorm, GetActorUpVector()).GetSafeNormal();
	FQuat DeltaZRotation = FQuat::FindBetweenNormals(
		GetActorForwardVector(),
		PlaneTarget);
	
	AddActorWorldRotation(FQuat::Slerp(FQuat::Identity, DeltaQuat, DeltaTime * RotationSpeed));
	AddActorWorldRotation(FQuat::Slerp(FQuat::Identity, DeltaZRotation, DeltaTime * RotationSpeed));
}

void AMultiLeggedPawn::FollowPath(double DeltaTime) {
	if(CurrentPathId < Path.Num()-1) {
		Rotate(DeltaTime, CurrentPathId+1);
		if(Move(DeltaTime, CurrentPathId+1)) {
			CurrentPathId++;
		}
	}
	else if(CurrentPathId == Path.Num()-1)
		Rotate(DeltaTime, CurrentPathId);
}

void AMultiLeggedPawn::BeginPlay() {
	LimbManager->AutoDetectLimbs(Mesh);
	ApplyGaitPreset(GaitPreset);
	Super::BeginPlay();
}

void AMultiLeggedPawn::Tick(float DeltaTime) {

	auto Whiskers = FibonacciTrace(Body->GetComponentLocation());
	// FVector WhiskerImpulse = GetClosestWhisker(Whiskers, true);
	FollowPath(DeltaTime);

	for(int i = 0; i < Path.Num(); ++i) {
		if(i < Path.Num()-1)
			DrawDebugLine(GetWorld(), Path[i].Origin, Path[i+1].Origin, FColor::Emerald);
		DrawDebugPoint(GetWorld(), Path[i].Origin, 5.f, FColor::Emerald);
	}
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
	
	float planeEq = Plane.PlaneDot(Body->GetComponentLocation());
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
	bool bSimplified = true;
	while(k < Path.Num()-2) {
		k++;
		FVector CurrentDirection = Path[k].Tangent;
		FVector NextDirection = Path[k+1].Tangent ;

		if(CurrentDirection.Dot(NextDirection) > SimplificationThreshold) {
			Path.RemoveAt(k);
			bSimplified = false;
			k--;
		}
		if(k >= Path.Num()-2 && !bSimplified) {
			bSimplified = true;
			k=0;
		}
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
		int kernel = max-min;
		float invKernelSize = 1.f/kernel;
		FPathNode Node;
		for(int j = max-1; j >=min; --j) {
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

