// Copyright Epic Games, Inc. All Rights Reserved.

#include "Entomon.h"

#define LOCTEXT_NAMESPACE "FEntomonModule"

DEFINE_LOG_CATEGORY(EntomonLog)

void FEntomonModule::StartupModule()
{
	UE_LOG(EntomonLog, Warning, TEXT("Entomon successfully loaded"));
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEntomonModule::ShutdownModule()
{
	UE_LOG(EntomonLog, Warning, TEXT("Entomon successfully shut down"));
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEntomonModule, Entomon)