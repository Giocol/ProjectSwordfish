#include "DownstairsGameMode.h"

#include "NoiseSystem.h"

void ADownstairsGameMode::InitGameState() {
	Super::InitGameState();
	if(NoiseSystemClass)
		NoiseSystemRef = GetWorld()->SpawnActor<ANoiseSystem>(NoiseSystemClass);
	else
		UE_LOG(LogTemp, Error, TEXT("ERROR: Missing NoiseSystemClass, plug it in the editor!"));
}
