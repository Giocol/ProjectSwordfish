// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entomon/Navigation/PathPreference.h"
#include "GameFramework/Pawn.h"
#include "Entomon/Navigation/NavNode.h"

#include "MultiLeggedPawn.generated.h"

class UGaitPreset;
class UFloatingPawnMovement;
class UMovementComponent;
class UProceduralLimbManager;

UCLASS()
class ENTOMON_API AMultiLeggedPawn : public APawn
{
	GENERATED_BODY()

public:
	AMultiLeggedPawn();
	
	UFUNCTION(BlueprintCallable)
		void SetPath(TArray<FNavNode> Nodes) {
		Path = Nodes;
		CurrentPathId = 0;
	}

	class UPoseableMeshComponent* GetMesh() { return Mesh; }

protected:
	virtual void BeginPlay() override;

	bool Move(double DeltaTime, int Target);
	void Rotate(double DeltaTime, int Target);
	void FollowPath(double DeltaTime);
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ApplyGaitPreset(UGaitPreset* InGaitPreset);

protected:
	bool Trace(FVector Start, FVector Direction, FHitResult& OutHit);
	TArray<FHitResult> FibonacciTrace(FVector Start);
	FVector InterpretWhiskers(TArray<FHitResult> Hits, bool bDraw);

	float GetNormalizedOffsetFromPreference(float Distance);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		UFloatingPawnMovement* MovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", UIMin = 0.0001f, UIMax = 1.f))
		float FacingBias = 0.1f; // Defines the importance of facing the target the pawn wants to approach
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float RotationSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		UGaitPreset* GaitPreset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		FPathPreference PathPreference;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		int NumWhiskers = 32;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float WhiskerRayDistance = 500;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float PreferredPersonalSpace = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float MinDistanceFromObstacle = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float MaxDistanceFromObstacle = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;

	TArray<FNavNode> Path;
	int CurrentPathId;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		UProceduralLimbManager* LimbManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		class UPoseableMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		class USceneComponent* Body;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		USceneComponent* Root;
	
};
