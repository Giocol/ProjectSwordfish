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
	if(BoundingVolumes.IsEmpty())
		return;
	for (int i = 0; i < Nodes.Num(); ++i) {
		auto BoundingVolume = Nodes[i];
		DrawDebugPoint(GetWorld(), BoundingVolume.Origin, 3.f, FColor::Emerald);
	}

	// AActor* Spider = UGameplayStatics::GetActorOfClass(GetWorld(), AMultiLeggedPawn::StaticClass());
	// int Id = GetClosestBoundingVolume(Spider->GetActorLocation(), true);
}

int ANavVolume::GetClosestBoundingVolume(FVector Point, bool bDraw) const {
	if(BoundingVolumes.IsEmpty() || BoundingVolumes[0].GetSignedDistance(Point) > 0)
		return INDEX_NONE;
		
	int Current = 0;
	while(true) {
		if(bDraw) 
			DrawDebugBox(GetWorld(), BoundingVolumes[Current].GetOrigin(), BoundingVolumes[Current].GetExtent(), FColor::White);

		auto children = BoundingVolumes[Current].GetChildren();
		bool bFound = false;
		if(children.IsEmpty())
			return Current;
		for (auto Child : children) {
			if(BoundingVolumes[Child].GetSignedDistance(Point) < 0) {
				Current = Child;
				bFound = true;
				break;
			}
		}
		if(!bFound)
			return INDEX_NONE;
	}
}

void ANavVolume::BeginPlay() {
	Super::BeginPlay();

	GenerateRoot();
	Populate();
}

void ANavVolume::GenerateRoot() {
	BoundingVolumes.Empty();
	auto Bounds = GetBounds();
	BoundingVolumes.Add(FBoundingVolume(Bounds.BoxExtent, Bounds.Origin));
}

void ANavVolume::Populate() {
	if(BoundingVolumes.IsEmpty())
		return;

	FVector RootExtent = BoundingVolumes[0].GetExtent();
	FVector RootOrigin = BoundingVolumes[0].GetOrigin();

	float len = RootExtent.Length();
	Nodes.Reserve(len * len * len / (Resolution * Resolution * Resolution));
	FVector CurrentPosition;
	for(CurrentPosition.X = -RootExtent.X; CurrentPosition.X <= RootExtent.X; CurrentPosition.X += Resolution) {
		for(CurrentPosition.Y = -RootExtent.Y; CurrentPosition.Y <= RootExtent.Y; CurrentPosition.Y += Resolution) {
			for(CurrentPosition.Z = -RootExtent.Z; CurrentPosition.Z <= RootExtent.Z; CurrentPosition.Z += Resolution) {
				Nodes.Add(FNavNode(CurrentPosition + RootOrigin));
			}
		}
	}
}
