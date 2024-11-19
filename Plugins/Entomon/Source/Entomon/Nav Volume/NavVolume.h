#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "NavVolume.generated.h"

UCLASS()
class ENTOMON_API ANavVolume : public AVolume {
	GENERATED_BODY()

public:
	ANavVolume();

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;
	
	FVector EvaluateNodeDistance(FVector At);

	void Populate();
	void SiftHeap(int FromId = 0, int Depth = 0);
	void Insert(struct FNavNode Node);
	
	void Connect();

	TArray<int> FindPath(int Start, int End);
	TArray<int> ReconstructPath(TMap<int, int> PathMap, int Current);
	float Heuristic(int Start, int End);

	int FindClosestNode(FVector Location);
	TArray<int> FindNodesWithin(FVector Location, float WithinRadius);

private:
	bool Trace(FVector Start, FVector Direction, FHitResult& OutHit);
	bool TraceFibonacciSphere(FVector Start, TArray<FHitResult>& OutHit);

	FORCEINLINE int GetParentId(int Id) { return Id > 0 ? (Id-1)/2 : INDEX_NONE; }
	FORCEINLINE int GetAxisParentId(int Id) { return GetParentId(GetParentId(GetParentId(Id))); }
	FORCEINLINE int GetDepth(int AtId) {
		int Result = 0;
		while(AtId > 0) {
			AtId = GetParentId(AtId);
			Result++;
		}
		return Result;
	}
	FORCEINLINE int GetAxis(int AtId) { return GetDepth(AtId) % 3; }
	TArray<int> GetDescendants(int AtId, int Axis = INDEX_NONE);
	

	FHitResult GetClosestHit(TArray<FHitResult> InHits);
public:
	UPROPERTY(EditAnywhere)
		float Resolution = 50.f;
	UPROPERTY(EditAnywhere)
		int TraceIterations = 64;
	UPROPERTY(EditAnywhere)
		float TraceDistance = 256.f;
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;
	UPROPERTY(EditAnywhere)
		bool bConnectDiagonal = true;
	
	TArray<struct FNavNode> Nodes;
	int NavTreeRoot = 0;
};
