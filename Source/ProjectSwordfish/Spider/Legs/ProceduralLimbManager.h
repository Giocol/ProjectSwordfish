// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Limb.h"

#include "ProceduralLimbManager.generated.h"

USTRUCT(Blueprintable)
struct FBounds {
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
		FVector2D Min = FVector2D(-100.f, -100.f);
	UPROPERTY(EditDefaultsOnly)
		FVector2D Max= FVector2D(100.f, 100.f);
};

UCLASS(ClassGroup=(Locomotion), meta=(BlueprintSpawnableComponent))
class PROJECTSWORDFISH_API UProceduralLimbManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UProceduralLimbManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool FindLegs();
	void FindHip(FName From);

	void GetBoundsCornersWorld(FVector& BottomLeft, FVector& BottomRight, FVector& TopLeft, FVector& TopRight) const;
	TArray<FVector> GetBoundsCornersWorldArray() const;
	FVector GetBoundsScale() const;
	
	void SetBonePositions(bool bDraw = false);

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion", meta=(AllowPrivateAccess="true"))
		FBounds Bounds;
	
	UPROPERTY(EditDefaultsOnly)
		FName HipJointsName = "Hip";
	UPROPERTY(EditDefaultsOnly)
		FName EndEffectorsName = "Tip";

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<FLeg> Legs;
	
	class UPoseableMeshComponent* Mesh;
};


