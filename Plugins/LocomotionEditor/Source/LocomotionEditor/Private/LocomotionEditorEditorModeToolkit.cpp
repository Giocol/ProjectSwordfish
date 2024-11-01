// Copyright Epic Games, Inc. All Rights Reserved.

#include "LocomotionEditorEditorModeToolkit.h"
#include "LocomotionEditorEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "LocomotionEditorEditorModeToolkit"

FLocomotionEditorEditorModeToolkit::FLocomotionEditorEditorModeToolkit()
{
}

void FLocomotionEditorEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FLocomotionEditorEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FLocomotionEditorEditorModeToolkit::GetToolkitFName() const
{
	return FName("LocomotionEditorEditorMode");
}

FText FLocomotionEditorEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "LocomotionEditorEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
