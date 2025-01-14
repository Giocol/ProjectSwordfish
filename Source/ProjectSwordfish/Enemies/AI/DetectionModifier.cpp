#include "DetectionModifier.h"

UDetectionModifier::UDetectionModifier() {
	PrimaryComponentTick.bCanEverTick = false;

}

void UDetectionModifier::SetDetectionSignalModifier(float Value)
{
	DefaultSignalModifier = Value;
}

