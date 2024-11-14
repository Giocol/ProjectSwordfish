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

	int GetClosestBoundingVolume(FVector Point, bool bDraw = false) const;
protected:
	virtual void BeginPlay() override;
	
	void GenerateRoot();

	void Populate();

public:
	UPROPERTY(EditAnywhere)
		float Resolution = 50.f;
	
	TArray<struct FNavNode> Nodes;
	TArray<struct FBoundingVolume> BoundingVolumes;
};
