#include "NavVolume.h"

#include "BoundingVolume.h"
#include "NavData.h"
#include "NavNode.h"
#include "PathPreference.h"
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
	
	DrawNodes();

	// AActor* Spider = UGameplayStatics::GetActorOfClass(GetWorld(), AMultiLeggedPawn::StaticClass());
	// int Id = FindClosestNode(Spider->GetActorLocation());
	// if(Id >= 0 && Id < Nodes.Num())
	// 	DrawDebugBox(GetWorld(), Nodes[Id].Origin, FVector::OneVector * 15, FColor::Yellow);
	// FPathPreference testPref = {0, 100, 50, 0.5 };
	// auto Path = FindPath(Id, FindClosestNode(TestTarget), testPref);
	//
	// if(Path.Num() < 2)
	// 	return;
	// for(int i = 1; i < Path.Num(); ++i) {
	// 	DrawDebugLine(GetWorld(), Nodes[Path[i-1]].Origin, Nodes[Path[i]].Origin, FColor::Yellow);
	// }
}

TArray<FNavNode> ANavVolume::FindPath(FVector Start, FVector Target, FPathPreference PathPreference) {
	auto indices = FindPath(FindClosestNode(Start), FindClosestNode(Target), PathPreference);
	TArray<FNavNode> Result;
	Result.Reserve(indices.Num());
	for(int i = 0; i < indices.Num(); ++i)
		Result.Add(Nodes[indices[i]]);
	return Result;
}

void ANavVolume::BuildNavigationData() {
	auto NavDataPtr = NavigationData.LoadSynchronous();
	if(NavDataPtr) {
		FDateTime CurrentDateAndTime = FDateTime::UtcNow();
		Populate();
		Connect();
		FDateTime NewTime = FDateTime::UtcNow();
		auto diff = NewTime - CurrentDateAndTime;
		float time = static_cast<float>(diff.GetSeconds()) + static_cast<float>(diff.GetFractionMilli()) / 1000;
		NavDataPtr->Write(Nodes);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Navigation Data Reference has not been set. Cancelling build..."));
	}
}

void ANavVolume::LoadNavigationData() {
	auto NavDataPtr = NavigationData.LoadSynchronous();
	if(NavDataPtr)
		NavDataPtr->Read(Nodes);
	else {
		UE_LOG(LogTemp, Warning, TEXT("Navigation Data Reference has not been set. Cancelling load..."));
	}
}

void ANavVolume::BeginPlay() {
	Super::BeginPlay();
	if(bForceRebuild)
		BuildNavigationData();
	else {
		LoadNavigationData();
		if(Nodes.IsEmpty() && bRebuildOnEmptyLoad)
			BuildNavigationData();
			
	}
}

FVector ANavVolume::EvaluateNodeDistance(FVector At) {
	TArray<FHitResult> Hits;
	if(FibonacciTrace(At, Hits)) {
		FHitResult Closest = InterpretTraces(Hits);
		if(!Closest.Location.IsZero())
			return Closest.Location - At;
	}
	return FVector::ZeroVector;
}

void ANavVolume::Populate() {

	FVector RootExtent = GetActorScale().GetAbs() * 100;
	FVector RootOrigin = GetActorLocation();

	Nodes.Reserve(8 * RootExtent.X * RootExtent.Y * RootExtent.Z / (Resolution * Resolution * Resolution));
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
				float Distance;
				FVector Normal;
				Displacement.ToDirectionAndLength(Normal,Distance);
				if(Distance > 0) {
					auto Node = FNavNode(WorldPos);
					Node.Normal = -Normal;
					Node.Distance = Distance;
					Nodes.Add(Node);
				}
			}
		}
	}
	Nodes.Shrink();
}

void ANavVolume::SiftHeap(int FromId, int Depth) {
	if(Nodes.IsEmpty())
		return;

	int LeftChild = 2 * FromId + 1;
	int RightChild = 2 * FromId + 2;

	int Axis = Depth % 3;
	Depth++;

	if(LeftChild < Nodes.Num()) {
		if(Nodes[LeftChild].Origin[Axis] < Nodes[FromId].Origin[Axis]) {
			auto temp = Nodes[LeftChild];
			Nodes[LeftChild] = Nodes[FromId];
			Nodes[FromId] = temp;
		}
		SiftHeap(LeftChild, Depth);
	}
	if(RightChild < Nodes.Num()) {
		if(Nodes[RightChild].Origin[Axis] < Nodes[FromId].Origin[Axis]) {
			auto temp = Nodes[RightChild];
			Nodes[RightChild] = Nodes[FromId];
			Nodes[FromId] = temp;
		}
		SiftHeap(RightChild, Depth);
	}
	
	// bool bCorrectAxis = GetAxis(LeftChild) != RootAxis || GetAxis(RightChild) != RootAxis;
	// int newRoot = bCorrectAxis ? FromId : RootId;
	// if(LeftChild < Nodes.Num())
	// 	SiftHeap(LeftChild, newRoot);
	// if(RightChild < Nodes.Num())
	// 	SiftHeap(RightChild, newRoot);
	// if( FromId != RootId &&
	// 	FromId < Nodes.Num() &&
	// 	Nodes[FromId].Origin[RootAxis] < Nodes[RootId].Origin[RootAxis])
	// {
	// 	FNavNode temp = Nodes[FromId];
	// 	Nodes[FromId] = Nodes[RootId];
	// 	Nodes[RootId] = temp;
	// }

	// int ParentId = GetAxisParentId(FromId);
	// if(ParentId >= 0) {
	// 	int Axis = GetAxis(ParentId);
	// 	if(Nodes[ParentId].Origin[Axis] > Nodes[FromId].Origin[Axis]) {
	// 		Nodes.Swap(FromId, ParentId);
	// 		SiftHeap(ParentId);
	// 	}
	// }
	
	// TArray<FNavNode> Result = Nodes;
	// Nodes.Empty();
	// for(int i = 0; i < Result.Num(); ++i) {
	// 	(Result[i]);
	// }
	//
	// int Depth = 0;
	// Nodes.Heapify([&Depth, this](FNavNode lhs, FNavNode rhs) {
	// 	const int Axis = Depth % 3;
	// 	Depth++;
	// 	return lhs.Origin[Axis] < rhs.Origin[Axis];
	// });
}

void ANavVolume::Insert(FNavNode Node) {
	Nodes.Add(Node);
	SiftHeap();
	// SiftHeap(Nodes.Num()-1);
	// if(Nodes.Num() < 2) {
	// 	Nodes.Add(Node);
	// 	return;
	// }
	// int SelfId = Nodes.Num() - 1;
	// int ParentId = GetParentId(SelfId);

	// int Axis = 0;
	// while(SelfId != 0 && Nodes[ParentId].Origin[Axis] > Nodes[SelfId].Origin[Axis]) {
	// 	FNavNode temp = Nodes[SelfId];
	// 	Nodes[SelfId] = Nodes[ParentId];
	// 	SelfId = ParentId;
	// 	ParentId = GetParentId(SelfId);
	// 	Axis = GetAxis(SelfId);
	// 	Nodes[SelfId] = temp;
	// }
}

void ANavVolume::Connect() {
	TArray<AActor*> Ignore;
	for(int i = 0; i < Nodes.Num(); ++i) {
		float Within = bConnectDiagonal ? UE_SQRT_3 * Resolution * 1.01 : Resolution * 1.01;
		for (auto NodeId : FindNodesWithin(Nodes[i].Origin, Within)) {
			FHitResult Hit;
			bool bHit = UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				Nodes[i].Origin, Nodes[NodeId].Origin,
				UEngineTypes::ConvertToTraceType(TraceChannel),
				false,
				Ignore,
				EDrawDebugTrace::None,
				Hit, true
				);
			if(!bHit) {
				float Distance = FVector::Distance(Nodes[i].Origin, Nodes[NodeId].Origin);
				Nodes[i].Connect(FNavLink(NodeId, Distance));
			}
		}
	}
}

TArray<int> ANavVolume::ReconstructPath(TMap<int, int> PathMap, int Current) {
	TArray<int> Result;
	while(PathMap.Contains(Current)) {
		Current = PathMap[Current];
		Result.Insert(Current, 0);
	}
	return Result;
}

int ANavVolume::FindClosestToTarget(TMap<int, int> PathMap, int End) {
	int Closest = 0;
	float closestDist = INFINITY;
	for (auto i : PathMap) {
		float dist = FVector::DistSquared(Nodes[i.Value].Origin, Nodes[End].Origin);
		if(dist < closestDist) {
			closestDist = dist;
			Closest = i.Value;
		}
	}
	return Closest;
}

TArray<int> ANavVolume::FindPath(int Start, int End, FPathPreference PathPreference) {
	TArray<FNavLink> OpenSet = { FNavLink(Start, 0) };
	TMap<int, float> gScore = { TTuple<int, float>(Start, 0) };
	TMap<int, float> fScore = { TTuple<int, float>(Start, Heuristic(Start, End)) };
	TMap<int, int> CameFrom;

	int LastId = Start;
	while(!OpenSet.IsEmpty()) {
		FNavLink Current = OpenSet[0];
		for (auto Entry : OpenSet) {
			if(fScore.Contains(Entry.Id)) {
				if(fScore[Entry.Id] < fScore[Current.Id])
					Current = Entry;
			}
		}
		// DrawDebugLine(GetWorld(), Nodes[LastId].Origin, Nodes[Current.Id].Origin, FColor::Silver);
		
		if(Current.Id == End) {
			return ReconstructPath(CameFrom, Current.Id);
		}
		OpenSet.Remove(Current);
		for(FNavLink& Link : Nodes[Current.Id].Connections) {
			
			float Tentative_gScore = gScore[Current.Id] + Link.Distance;
			if(!gScore.Contains(Link.Id))
				gScore.Add(Link.Id, INFINITY);
			if(Tentative_gScore < gScore[Link.Id]) {
				CameFrom.Add(Link.Id, Current.Id);

				gScore.Add(Link.Id, Tentative_gScore);
				bool bIsReachable =
					Nodes[Link.Id].Distance > PathPreference.MinDistance &&
					Nodes[Link.Id].Distance < PathPreference.MaxDistance;
				if(!fScore.Contains(Link.Id) && bIsReachable) {
					float upDot = FVector::UpVector.Dot(Nodes[Link.Id].Normal);
					float remappedDot = 0.5*(1-upDot);
					remappedDot *= remappedDot;
					float newFScore = Tentative_gScore + Heuristic(Link.Id, End) + PathPreference.UpPreference * remappedDot;
					// if(PathPreference.MaxDistance < INFINITY) {
					// 	float absDistFromPreference = FMath::Abs(Nodes[Link.Id].Distance - PathPreference.PreferredDistance);
					// 	float distNormalized =
					// 		(absDistFromPreference - PathPreference.MinDistance) / (PathPreference.MaxDistance / PathPreference.MinDistance);
					// 	float TangentialDist = 2 * FMath::Tan(HALF_PI * distNormalized) * INV_PI;
					// 	// float biasedPreference = FMath::Pow(distNormalized, (1-PathPreference.PreferenceWeight)/PathPreference.PreferenceWeight);
					// 	newFScore += PathPreference.PreferenceWeight * TangentialDist;
					// }
					fScore.Add(Link.Id, newFScore);
				}
				if(bIsReachable) {
					bool bContains = false;
					for(auto Entry : OpenSet)
						if(Entry.Id == Link.Id) { bContains = true; break; }
					if(!bContains)
						OpenSet.Add(Link);
				}
			}
		}
		LastId = Current.Id;
	}
	return ReconstructPath(CameFrom, FindClosestToTarget(CameFrom, End));
}

float ANavVolume::Heuristic(int Start, int End) {
	if(Start > 0 && End > 0 && !Nodes.IsEmpty())
		return FVector::Distance(Nodes[Start].Origin, Nodes[End].Origin);
	else return INFINITY;
}

int ANavVolume::FindClosestNode(FVector Location) {

	if(Nodes.Num() == 0)
		return INDEX_NONE;
	else if(Nodes.Num() == 1)
		return 0;
		
	int ClosestId = 0;
	float ClosestSqrDist = INFINITY;
	for(int i = 1; i < Nodes.Num(); ++i) {
		float CurrentSqrDist = FVector::DistSquared(Location, Nodes[i].Origin);
		if(CurrentSqrDist < ClosestSqrDist) {
			ClosestId = i;
			ClosestSqrDist = CurrentSqrDist;
		}
	}
	return ClosestId;
	// int CurrentId = 0;
	//
	// int k = 0;
	// while(true) {
	// 	if(
	// 		FMath::Abs((Nodes[CurrentId].Origin.X - Location.X)) < Resolution / 2 &&
	// 		FMath::Abs((Nodes[CurrentId].Origin.Y - Location.Y)) < Resolution / 2 &&
	// 		FMath::Abs((Nodes[CurrentId].Origin.Z - Location.Z)) < Resolution / 2)
	// 	{
	// 		return CurrentId;
	// 	}
	// 		
	// 	int lhs = (2 * CurrentId+1);
	// 	int rhs = (2 * CurrentId+2);
	// 	if(lhs >= Nodes.Num() || rhs >= Nodes.Num())
	// 		break;
	//
	// 	int LastId = CurrentId;
	// 	int Axis = k % 3;
	// 	if(		FMath::Abs(Nodes[lhs].Origin[Axis] - Location[Axis]) <
	// 			FMath::Abs(Nodes[rhs].Origin[Axis] - Location[Axis]))
	// 		CurrentId = rhs;
	// 	else
	// 		CurrentId = lhs;
	// 	
	// 	
	// 	DrawDebugLine(GetWorld(), Nodes[LastId].Origin, Nodes[CurrentId].Origin, FColor::White);
	// 	k++;
	// 	if(k > Nodes.Num()) break;
	// }
	//
	// return INDEX_NONE;
}

TArray<int> ANavVolume::FindNodesWithin(FVector Location, float WithinRadius) {
	TArray<int> Result;
	for(int i = 1; i < Nodes.Num(); ++i) {
		float CurrentSqrDist = FVector::DistSquared(Location, Nodes[i].Origin);
		if(!FMath::IsNearlyZero(CurrentSqrDist) && CurrentSqrDist < WithinRadius * WithinRadius) {
			Result.Add(i);
		}
	}
	return Result;
}

bool ANavVolume::Trace(FVector Start, FVector Direction, FHitResult& OutHit) {
	TArray<AActor*> Ignore;
	FVector End = Start + Direction * TraceDistance;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start, End,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		Ignore,
		EDrawDebugTrace::None,
		OutHit,
		true);
	FHitResult BackTrace;
	if(bHit) {
		bool bBackTraceHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		OutHit.Location, Start,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		true,
		Ignore,
		EDrawDebugTrace::None,
		BackTrace,
		true);
		if(bBackTraceHit)
			return false;
	}
	return bHit;
}

bool ANavVolume::FibonacciTrace(FVector Start, TArray<FHitResult>& OutHit) {
	bool Result = false;
	for(int i = 0; i < TraceIterations; i++) {
		float theta = TWO_PI * i / UE_GOLDEN_RATIO;
		float phi = FMath::Acos(1 - (2.0 * i) / TraceIterations);
		FVector Direction = -FVector(
			FMath::Cos(theta) * FMath::Sin(phi),
			FMath::Sin(theta) * FMath::Sin(phi),
			FMath::Cos(phi)
			);
		OutHit.Add(FHitResult());
		bool bHit = Trace(Start, Direction, OutHit.Last());
		if(bHit && !Result)
			Result = true;
	}
	return Result;
}

void ANavVolume::DrawNodes() {
	FColor FarColor = FColor::Emerald;
	FColor NearColor = FColor::Black;
	for (int i = 0; i < Nodes.Num(); ++i) {
		if(bDrawNodes)
		{
			float alpha = FMath::Clamp(Nodes[i].Distance / TraceDistance, 0.f, 1.f);
			float invAlpha = 1.f - alpha;
			FColor Color = FColor(
				FarColor.R * invAlpha + NearColor.R * alpha,
				FarColor.G * invAlpha + NearColor.G * alpha,
				FarColor.B * invAlpha + NearColor.B * alpha
				);
			DrawDebugPoint(GetWorld(), Nodes[i].Origin, 3.f, Color);
		}
		if(bDrawNormals) {
			DrawDebugLine(GetWorld(), Nodes[i].Origin,
						Nodes[i].Origin - Nodes[i].Normal * Nodes[i].Distance,
						FColor::Blue);
		}
	}
}

TArray<int> ANavVolume::GetDescendants(int AtId, int Axis) {
	TArray<int> Descendants;

	int LeftChild = 2 * AtId + 1;
	int RightChild = 2 * AtId + 2;
	if(Axis == INDEX_NONE)
		Axis = GetAxis(AtId);
	
	if(LeftChild < Nodes.Num()) {
		int LeftAxis = GetAxis(LeftChild);
		if(LeftAxis == Axis)
			Descendants.Add(LeftChild);
		Descendants.Append(GetDescendants(LeftChild, Axis));
	}
	if(RightChild < Nodes.Num()) {
		int RightAxis = GetAxis(RightChild);
		if(RightAxis == Axis)
			Descendants.Add(RightChild);
		Descendants.Append(GetDescendants(RightChild, Axis));
	}

	
	return Descendants;
}

FHitResult ANavVolume::InterpretTraces(TArray<FHitResult> InHits) {
	FHitResult Closest;
	int Num=0;
	Closest.Distance = INFINITY;
	for (auto Hit : InHits) {
		if(Hit.Distance != 0 && Hit.Distance < Closest.Distance)
			Closest = Hit;
	}
	return Closest;
}
