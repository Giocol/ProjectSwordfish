#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESignalSeverity : uint8 {
	Nonperceptible = 0,
	Weak,
	Medium,
	Strong
};	