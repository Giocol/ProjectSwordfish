// Copyright Epic Games, Inc. All Rights Reserved.

#include "LocomotionEditorEditorModeCommands.h"
#include "LocomotionEditorEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "LocomotionEditorEditorModeCommands"

FLocomotionEditorEditorModeCommands::FLocomotionEditorEditorModeCommands()
	: TCommands<FLocomotionEditorEditorModeCommands>("LocomotionEditorEditorMode",
		NSLOCTEXT("LocomotionEditorEditorMode", "LocomotionEditorEditorModeCommands", "LocomotionEditor Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FLocomotionEditorEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FLocomotionEditorEditorModeCommands::GetCommands()
{
	return FLocomotionEditorEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
