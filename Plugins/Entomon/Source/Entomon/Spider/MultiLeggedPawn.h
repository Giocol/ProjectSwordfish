// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

	class UPoseableMeshComponent* GetMesh() { return Mesh; }

	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(FVector InLocation) { TargetLocation = InLocation; }
protected:
	virtual void BeginPlay() override;

	void Move(FVector Target);
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ApplyGaitPreset(UGaitPreset* InGaitPreset);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		UFloatingPawnMovement* MovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true", UIMin = 0.0001f, UIMax = 1.f))
		float FacingBias = 0.1f; // Defines the importance of facing the target the pawn wants to approach
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		float RotationSpeed = 360.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
		UGaitPreset* GaitPreset;
	FVector TargetLocation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		UProceduralLimbManager* LimbManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		class UPoseableMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		class UBoxComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
		USceneComponent* Root;
	
};
