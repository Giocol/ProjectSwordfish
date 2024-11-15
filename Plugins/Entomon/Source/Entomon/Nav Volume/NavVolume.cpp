#include "NavVolume.h"

#include "BoundingVolume.h"
#include "NavNode.h"
#include "Components/BrushComponent.h"
#include "Entomon/Spider/MultiLeggedPawn.h"
#include "Kismet/GameplayStatics.h"

ANavVolume::ANavVolume() {
	GetBrushComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetBrushComponent()->SetGenerateOverlapEvents(false);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ANavVolume::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	
	for (int i = 0; i < Nodes.Num(); ++i) {
		FColor FarColor = FColor::Green;
		FColor NearColor = FColor::Black;
		float alpha = FMath::Clamp(Nodes[i].DistanceFromNearestSurface / TraceDistance, 0.f, 1.f);
		float invAlpha = 1.f - alpha;
		FColor Color = FColor(
			FarColor.R * invAlpha + NearColor.R * alpha,
			FarColor.G * invAlpha + NearColor.G * alpha,
			FarColor.B * invAlpha + NearColor.B * alpha
			);
		
		// DrawDebugLine(GetWorld(), Nodes[i].Origin, Nodes[i].Origin + Nodes[i].DisplacementFromNearestSurface, Color);
		DrawDebugPoint(GetWorld(), Nodes[i].Origin, 4.f, Color);
	}

	AActor* Spider = UGameplayStatics::GetActorOfClass(GetWorld(), AMultiLeggedPawn::StaticClass());
	int Id = FindClosestNode(Spider->GetActorLocation());
}

void ANavVolume::BeginPlay() {
	Super::BeginPlay();

	FDateTime CurrentDateAndTime = FDateTime::UtcNow();
	Populate();
	TreeifyNodes();
	FDateTime NewTime = FDateTime::UtcNow();
	auto diff = NewTime - CurrentDateAndTime;
	float time = static_cast<float>(diff.GetSeconds()) + static_cast<float>(diff.GetFractionMilli()) / 1000;
}

FVector ANavVolume::EvaluateNodeDistance(FVector At) {
	TArray<FHitResult> Hits;
	if(TraceFibonacciSphere(At, Hits)) {
		FHitResult Closest = GetClosestHit(Hits);
		if(!Closest.Location.IsZero())
			return Closest.Location - At;
	}
	return FVector::ZeroVector;
}

void ANavVolume::Populate() {

	FVector RootExtent = GetActorScale().GetAbs() * 100;
	FVector RootOrigin = GetActorLocation();

	float len = RootExtent.Length();
	Nodes.Reserve(len * len * len / (Resolution * Resolution * Resolution));
	FVector CurrentPosition;
	FIntVector Num = FIntVector(
		FMath::Floor(RootExtent.X / Resolution),
		FMath::Floor(RootExtent.Y / Resolution),
		FMath::Floor(RootExtent.Z / Resolution)
		);
	
	for(CurrentPosition.X = -RootExtent.X; CurrentPosition.X <= RootExtent.X; CurrentPosition.X += Resolution) {
		for(CurrentPosition.Y = -RootExtent.Y; CurrentPosition.Y <= RootExtent.Y; CurrentPosition.Y += Resolution) {
			for(CurrentPosition.Z = -RootExtent.Z; CurrentPosition.Z <= RootExtent.Z; CurrentPosition.Z += Resolution) {
				FVector WorldPos = GetTransform().GetRotation() * CurrentPosition + RootOrigin;
				FVector Displacement = EvaluateNodeDistance(WorldPos);
				float Distance = Displacement.Length();
				if(Distance > 0) {
					auto Node = FNavNode(WorldPos);
					Node.DisplacementFromNearestSurface = Displacement;
					Node.DistanceFromNearestSurface = Distance;
					Nodes.Add(Node);
				}
			}
		}
	}
	Nodes.Shrink();
}

void ANavVolume::TreeifyNodes() {
	if(Nodes.IsEmpty())
		return;

	int Depth = 0;
	Nodes.Heapify([&Depth, this](FNavNode lhs, FNavNode rhs) {
		const int Axis = Depth % 3;
		Depth++;
		return lhs.Origin[Axis] < rhs.Origin[Axis];
	});
	// NavTreeRoot = Nodes.Num() / 2;
}

void ANavVolume::Connect() {
	
}

int ANavVolume::FindClosestNode(FVector Location) {
	int CurrentId = 0;

	int k = 0;
	while(true) {
		if(FVector::DistSquared(Nodes[CurrentId].Origin, Location) < Resolution * Resolution)
			return CurrentId;
			
		int lhs = (2 * CurrentId+1) % Nodes.Num();
		int rhs = (2 * CurrentId+2) % Nodes.Num();

		int LastId = CurrentId;
		int Axis = k % 3;
		if(Nodes[lhs].Origin[Axis] < Location[Axis])
			CurrentId = rhs;
		else
			CurrentId = lhs;
		
		
		DrawDebugLine(GetWorld(), Nodes[LastId].Origin, Nodes[CurrentId].Origin, FColor::White);
		k++;
		if(k > 100) break;
	}
	
	return INDEX_NONE;
}

bool ANavVolume::Trace(FVector Start, FVector Direction, FHitResult& OutHit) {
	TArray<AActor*> Ignore;
	FVector End = Start + Direction * TraceDistance;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start, End,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		true,
		Ignore,
		EDrawDebugTrace::None,
		OutHit,
		true);
	
	return bHit;
}

bool ANavVolume::TraceFibonacciSphere(FVector Start, TArray<FHitResult>& OutHit) {
	for(int i = 0; i < TraceIterations; i++) {
		float theta = TWO_PI * i / UE_GOLDEN_RATIO;
		float phi = FMath::Acos(1 - (2.0 * i) / TraceIterations);
		FVector Direction = -FVector(
			FMath::Cos(theta) * FMath::Sin(phi),
			FMath::Sin(theta) * FMath::Sin(phi),
			FMath::Cos(phi)
			);
		OutHit.Add(FHitResult());
		Trace(Start, Direction, OutHit.Last());
	}
	return true;
}

FHitResult ANavVolume::GetClosestHit(TArray<FHitResult> InHits) {
	FHitResult Closest;
	Closest.Distance = INFINITY;
	for (auto Hit : InHits) {
		if(Hit.Distance != 0 && Hit.Distance < Closest.Distance)
			Closest = Hit;
	}
	return Closest;
}
