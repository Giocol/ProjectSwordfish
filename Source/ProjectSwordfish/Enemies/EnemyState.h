#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EEnemyState : uint8 {
	Idle = 0,
	Suspicious,
	Agitated
};