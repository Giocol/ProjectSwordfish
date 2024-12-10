#include "Ocean.h"
#include "NiagaraComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2DArray.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

AOcean::AOcean() {
	SetDefaultOceanParameters();
	OceanSimulator = CreateDefaultSubobject<UNiagaraComponent>("Simulator");
	SetNiagaraParameters();
	ReadCompleteDelegate.BindUObject(this, &AOcean::OnReadComplete);
	
	PrimaryActorTick.bCanEverTick = true;
}

void AOcean::BeginPlay() {
	Super::BeginPlay();
	SetNiagaraParameters();
	LoadAllRenderTextures();

	EvaluatedDisplacement.Reserve(sqrtLengthDisps*sqrtLengthDisps);
	for(int i = 0; i < sqrtLengthDisps*sqrtLengthDisps; i++)
		EvaluatedDisplacement.Add(FVector::Zero());

	PositionsToEvaluate.Reserve(sqrtLengthDisps*sqrtLengthDisps);
	for(int i = 0; i < sqrtLengthDisps; i++) {
		for(int j = 0; j < sqrtLengthDisps; j++) {
			FVector v = FVector(i, j, 0) * 1000;
			PositionsToEvaluate.Add(v);
		}
	}
}

void AOcean::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	UnloadAllRenderTextures();
	Super::EndPlay(EndPlayReason);
}

void AOcean::Tick(float DeltaTime) {

	//float d = EvaluateOceanHeight(FVector2f::One() * 100);
	//DrawDebugBox(GetWorld(), FVector(100, 100, d), FVector::One() * 1000, FColor::Green);
	
	//ReadOceanData(VertexDataA);
	//if(bShowGizmos) {
	//	for(int i = 0; i < PositionsToEvaluate.Num(); i++) {
	//		if(i > EvaluatedDisplacement.Num())
	//			break;
	//		DrawDebugBox(
	//			GetWorld(),
	//			PositionsToEvaluate[i] + EvaluatedDisplacement[i] * 0.01, FVector::One()*100,
	//			FColor::Green);
	//	}
	//}
	
	//UTextureRenderTarget2D* casc0 = VertexDataA->;
	//UAsyncReadRenderTargetAction::AsyncReadRenderTarget(GetWorld(), casc0, coords.X, coords.Y, false);
	//Waves->DoublePass();


	// if(bShowGizmos) {
	// 	for(int posX = 0; posX < sqrtLengthDisps; posX++) {
	// 		for(int posY = 0; posY < sqrtLengthDisps; posY++) {
	// 			FVector ownPos = PositionsToEvaluate[posY * sqrtLengthDisps + posX];
	// 			FVector ownDisp = Waves->GetWorldDisplacementAt(ownPos);
	// 			for(int offsetX = -1; offsetX < 2; offsetX++) {
	// 				for(int offsetY = -1; offsetY < 2; offsetY++) {
	// 					if(offsetX == 0 && offsetY == 0)
	// 						continue;
	// 					if(posX + offsetX > 32 || posX + offsetX < 0 ||
	// 						posY + offsetY > 32 || posY + offsetY < 0)
	// 						continue;
	// 					FVector otherPos = PositionsToEvaluate[(posY + offsetY) * sqrtLengthDisps + (posX + offsetX)];
	// 					FVector otherDisp = Waves->GetWorldDisplacementAt(otherPos);
	// 					DrawDebugLine(GetWorld(),
	// 						ownPos + ownDisp,
	// 						otherPos + otherDisp,
	// 						FColor::Green);
	// 				}
	// 			}
	// 		}
	// 		
	// 		//DrawDebugBox(
	// 		//	GetWorld(),
	// 		//	PositionsToEvaluate[i] + Waves->GetWorldDisplacementAt(PositionsToEvaluate[i]), FVector::One()*100,
	// 		//	FColor::Green);
	// 	}
	// }
	
	Super::Tick(DeltaTime);
}

float AOcean::EvaluateOceanHeight(FVector2f At) {
	if(!VertexDataA || !StreamHandle->HasLoadCompleted()) {
		return NAN;
	}
	
	return 0;
}

#if WITH_EDITOR
void AOcean::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	SetNiagaraParameters();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AOcean::SetDefaultOceanParameters() {
	Cascades.WindDirectionality = FVector4::One() * 0.5f;
	Cascades.Amplitude = FVector4(168000.f,  64000.f, 4000.f,  240.f);
	Cascades.Choppiness = FVector4::One() * 1.f;
	Cascades.PatchLength = FVector4(10.f,  28.f, 432.f,  2000.f);
	Cascades.ShortWaveCutoff = FVector4(0.0001f, 0.002f, 2.f, 30.f);
	Cascades.LongWaveCutoff = FVector4(1.0f, 0.25f, 0.125, 0.0625f);
	Cascades.WindTighten = FVector4::One() * 1.5f;

	Cascades.FoamInjection = FVector4::One() * 1.f;
	Cascades.FoamThreshold = FVector4::One() * -0.2f;
	Cascades.FoamFade = FVector4::One() * 0.1f;
	Cascades.FoamBlur = FVector4::One() * 1.f;

	OceanParameters.RoughnessSamples = 256;
	OceanParameters.RoughnessPower = 2.f;
	OceanParameters.WindSpeed = 15.f;
	OceanParameters.WindDirection = 0.f;
	OceanParameters.Gravity = 9.81f;
	OceanParameters.RepeatPeriod = 1000.f;
}

void AOcean::SetNiagaraParameters() {
	OceanSimulator->SetVariableVec4("WindDirectionality", Cascades.WindDirectionality);
	OceanSimulator->SetVariableVec4("Amplitude", Cascades.Amplitude);
	OceanSimulator->SetVariableVec4("Choppiness", Cascades.Choppiness);
	OceanSimulator->SetVariableVec4("PatchLength", Cascades.PatchLength);
	OceanSimulator->SetVariableVec4("ShortWaveCutoff", Cascades.ShortWaveCutoff);
	OceanSimulator->SetVariableVec4("LongWaveCutoff", Cascades.LongWaveCutoff);
	OceanSimulator->SetVariableVec4("WindTighten", Cascades.WindTighten);
	OceanSimulator->SetVariableVec4("FoamInjection", Cascades.FoamInjection);
	OceanSimulator->SetVariableVec4("FoamThreshold", Cascades.FoamThreshold);
	OceanSimulator->SetVariableVec4("FoamFade", Cascades.FoamFade);
	OceanSimulator->SetVariableVec4("FoamBlur", Cascades.FoamBlur);
	OceanSimulator->SetFloatParameter("WindSpeed", OceanParameters.WindSpeed);
	OceanSimulator->SetFloatParameter("WindDirection", OceanParameters.WindDirection);
	OceanSimulator->SetFloatParameter("Gravity", OceanParameters.Gravity);
	OceanSimulator->SetFloatParameter("RepeatPeriod", OceanParameters.RepeatPeriod);
	OceanSimulator->SetIntParameter("RoughnessSamples", OceanParameters.RoughnessSamples);
	OceanSimulator->SetFloatParameter("RoughnessPower", OceanParameters.RoughnessPower);
}

void AOcean::LoadAllRenderTextures() {
	// USwordfishGameInstance* GameInstance = Cast<USwordfishGameInstance>(GetGameInstance());
	// TArray paths = { VertexDataAPath, VertexDataBPath };
	// if(GameInstance) {
	// 	//VertexDataA = nullptr;
	// 	//VertexDataB = nullptr;
	// 	StreamHandle = GameInstance->StreamableManager.RequestAsyncLoad(
	// 		paths,
	// 		FStreamableDelegate::CreateUObject(this, &AOcean::OnLoadAllRenderTexturesComplete)
	// 		);
	// }
}

void AOcean::UnloadAllRenderTextures() {
	if(StreamHandle.IsValid())
		StreamHandle->ReleaseHandle();
}

void AOcean::OnLoadAllRenderTexturesComplete() {
	TArray<UObject*> LoadedAssets;
	StreamHandle->GetLoadedAssets(LoadedAssets);
	for(auto Object : LoadedAssets) {
		if(UTextureRenderTarget2DArray* asTexture = Cast<UTextureRenderTarget2DArray>(Object)) {
			FString Name = asTexture->GetName();
			if(Name == VertexDataAPath.GetAssetName()) {
				VertexDataA = asTexture;
			}
			else if(Name == VertexDataBPath.GetAssetName()) {
				VertexDataB = asTexture;
			}
		}
	}
}

void AOcean::ReadOceanData(UTextureRenderTarget2DArray* RenderTarget) {
	if(bIsLoading)
		return;
	bIsLoading = true;
	ENQUEUE_RENDER_COMMAND(ReadPixelDataCommand)([this, RenderTarget](FRHICommandListImmediate& CommandList) {
		ReadOceanData_RenderThread(RenderTarget);
	});
}

void AOcean::ReadOceanData_RenderThread(UTextureRenderTarget2DArray* RenderTarget) {
	FTextureRenderTargetResource* Resource = RenderTarget->GetRenderTargetResource();
	if(!Resource)
		return;
	uint32 stride = 0;
	TArray<FVector> Output;
	uint8* cascs[4];
	Output.Reserve(PositionsToEvaluate.Num());
	for(int i = 0; i < 4; i++) {
		cascs[i] = (uint8*)RHILockTexture2DArray(
		 Resource->GetTexture2DArrayRHI(),
		 i,
		 0,
		 RLM_ReadOnly,
		 stride,
		 false
		 );
		for(int j = 0; j < PositionsToEvaluate.Num(); j++) {
			FIntVector3 AsImageCoordinates = WorldToImageCoordinates(FVector2f(PositionsToEvaluate[j].X, PositionsToEvaluate[j].Y), i, Cascades);
			AsImageCoordinates = FIntVector3(AsImageCoordinates.X % RenderTarget->SizeX, AsImageCoordinates.Y % RenderTarget->SizeY, AsImageCoordinates.Z % RenderTarget->Slices);
			
			int pixelStartIndex = 4 * (AsImageCoordinates.Y * RenderTarget->SizeX + AsImageCoordinates.X);
			float X = static_cast<float>(cascs[i][pixelStartIndex])	  / 256;
			float Y = static_cast<float>(cascs[i][pixelStartIndex + 1]) / 256;
			float Z = static_cast<float>(cascs[i][pixelStartIndex + 2]) / 256;

			FVector disp = FVector(X, Y, Z) * Cascades.Amplitude[i];
			if(Output.Num() > j)
				Output[j] += disp;
			else
				Output.Emplace(disp);
		}
		RHIUnlockTexture2DArray(Resource->GetTexture2DArrayRHI(), i, 0, false);
	}
	
	
	ReadCompleteDelegate.ExecuteIfBound(Output);
}

void AOcean::OnReadComplete(TArray<FVector> Value) {
	EvaluatedDisplacement = Value;
	Value.Empty();
	bIsLoading = false;
}

FIntVector3 AOcean::WorldToImageCoordinates(FVector2f At, int CascadeIndex, FPerCascadeParameters CascadeInfo) {
	if(CascadeIndex >= 4)
		return FIntVector3::ZeroValue;

	const int texSize = 256;
	const int sqrTexSize = 256 * 256;
	
	FVector2f coords = At / CascadeInfo.PatchLength[CascadeIndex];
	if(coords.X > texSize)
		coords.X -= texSize;
	else if(coords.X < 0)
		coords.X += texSize;
	if(coords.Y > texSize)
		coords.Y -= texSize;
	else if(coords.Y < 0)
		coords.Y += texSize;
	
	FIntVector3 intCoords = FIntVector3(FMath::RoundToInt(coords.X), FMath::RoundToInt(coords.Y), CascadeIndex);
	return intCoords;
}





