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
	void TreeifyNodes();
	
	void Connect();

	int FindClosestNode(FVector Location);

private:
	bool Trace(FVector Start, FVector Direction, FHitResult& OutHit);
	bool TraceFibonacciSphere(FVector Start, TArray<FHitResult>& OutHit);

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
		bool ConnectDiagonal = true;
	
	TArray<struct FNavNode> Nodes;
	int NavTreeRoot = 0;
};
