// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Actor.h"
#include "OceanSimulationParameters.h"
#include "Ocean.generated.h"

class UTextureRenderTarget2DArray;

UCLASS()
class PROJECTSWORDFISH_API AOcean : public AActor
{
	GENERATED_BODY()

public:
	AOcean();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	float EvaluateOceanHeight(FVector2f At);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean Simulation", meta = (DisplayPriority=0))
		FPerCascadeParameters Cascades;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ocean Simulation", meta = (DisplayPriority=1))
		FOceanInfo OceanParameters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buoyancy", meta = (DisplayPriority=2))
		float FluidDensity = 0.997f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ocean Simulation",  meta = (DisplayPriority=100))
		TObjectPtr<class UNiagaraComponent> OceanSimulator;

	static FIntVector3 WorldToImageCoordinates(FVector2f At, int CascadeIndex, FPerCascadeParameters CascadeInfo);
	
private:
	void SetDefaultOceanParameters();
	void SetNiagaraParameters();
	void LoadAllRenderTextures();
	void UnloadAllRenderTextures();
	void OnLoadAllRenderTexturesComplete();
	void ReadOceanData(UTextureRenderTarget2DArray* RenderTarget);
	void ReadOceanData_RenderThread(UTextureRenderTarget2DArray* RenderTarget);
	void OnReadComplete(TArray<FVector> Value);
	
	UPROPERTY(EditDefaultsOnly)
		FSoftObjectPath VertexDataAPath;
	UPROPERTY(EditDefaultsOnly)
		FSoftObjectPath VertexDataBPath;
	UPROPERTY()
		TObjectPtr<class UTextureRenderTarget2DArray> VertexDataA;
	UPROPERTY()
		TObjectPtr<class UTextureRenderTarget2DArray> VertexDataB;

	UPROPERTY(EditInstanceOnly)
		bool bShowGizmos = true;
	

	TSharedPtr<struct FStreamableHandle> StreamHandle;

	TArray<FVector> EvaluatedDisplacement;
	TArray<FVector> PositionsToEvaluate;
	bool bIsLoading = false;
	
	const int sqrtLengthDisps = 50;
	
	class FBuoyancyThread* BuoyancyThread;
	FReadCompleteDelegate ReadCompleteDelegate;
};
