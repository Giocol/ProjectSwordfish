// Copyright Epic Games, Inc. All Rights Reserved.

#include "LocomotionEditorEditorMode.h"
#include "LocomotionEditorEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "LocomotionEditorEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/LocomotionEditorSimpleTool.h"
#include "Tools/LocomotionEditorInteractiveTool.h"

// step 2: register a ToolBuilder in FLocomotionEditorEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "LocomotionEditorEditorMode"

const FEditorModeID ULocomotionEditorEditorMode::EM_LocomotionEditorEditorModeId = TEXT("EM_LocomotionEditorEditorMode");

FString ULocomotionEditorEditorMode::SimpleToolName = TEXT("LocomotionEditor_ActorInfoTool");
FString ULocomotionEditorEditorMode::InteractiveToolName = TEXT("LocomotionEditor_MeasureDistanceTool");


ULocomotionEditorEditorMode::ULocomotionEditorEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(ULocomotionEditorEditorMode::EM_LocomotionEditorEditorModeId,
		LOCTEXT("ModeName", "LocomotionEditor"),
		FSlateIcon(),
		true);
}


ULocomotionEditorEditorMode::~ULocomotionEditorEditorMode()
{
}


void ULocomotionEditorEditorMode::ActorSelectionChangeNotify()
{
}

void ULocomotionEditorEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FLocomotionEditorEditorModeCommands& SampleToolCommands = FLocomotionEditorEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<ULocomotionEditorSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<ULocomotionEditorInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void ULocomotionEditorEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FLocomotionEditorEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> ULocomotionEditorEditorMode::GetModeCommands() const
{
	return FLocomotionEditorEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
