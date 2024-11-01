// Copyright Epic Games, Inc. All Rights Reserved.

#include "EntomonModule.h"
#include "EntomonEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "EntomonModule"

void FEntomonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FEntomonEditorModeCommands::Register();
}

void FEntomonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FEntomonEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEntomonModule, EntomonEditorMode)