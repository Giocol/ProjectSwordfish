#pragma once

#include "CoreMinimal.h"
#include "NavNode.h"
#include "UObject/Object.h"
#include "NavData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ENTOMON_API UNavData : public UDataAsset {
	GENERATED_BODY()
public:
	void Write(TArray<FNavNode> InNodes) { Nodes = InNodes; }
	void Read(TArray<FNavNode>& OutNodes) { OutNodes = Nodes; };
private:
	TArray<FNavNode> Nodes;
	
};
