// Copyright Epic Games, Inc. All Rights Reserved.

#include "EntomonEditorModeToolkit.h"
#include "EntomonEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "EntomonEditorModeToolkit"

FEntomonEditorModeToolkit::FEntomonEditorModeToolkit()
{
}

void FEntomonEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FEntomonEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FEntomonEditorModeToolkit::GetToolkitFName() const
{
	return FName("EntomonEditorMode");
}

FText FEntomonEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "EntomonEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
