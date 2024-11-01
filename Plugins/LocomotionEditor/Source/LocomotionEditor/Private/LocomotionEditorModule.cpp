// Copyright Epic Games, Inc. All Rights Reserved.

#include "LocomotionEditorModule.h"
#include "LocomotionEditorEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "LocomotionEditorModule"

void FLocomotionEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FLocomotionEditorEditorModeCommands::Register();
}

void FLocomotionEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FLocomotionEditorEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLocomotionEditorModule, LocomotionEditorEditorMode)