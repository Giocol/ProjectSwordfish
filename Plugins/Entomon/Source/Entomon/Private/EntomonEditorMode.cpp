// Copyright Epic Games, Inc. All Rights Reserved.

#include "EntomonEditorMode.h"
#include "EntomonEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "EntomonEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/EntomonSimpleTool.h"
#include "Tools/EntomonInteractiveTool.h"

// step 2: register a ToolBuilder in FEntomonEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "EntomonEditorMode"

const FEditorModeID UEntomonEditorMode::EM_EntomonEditorModeId = TEXT("EM_EntomonEditorMode");

FString UEntomonEditorMode::SimpleToolName = TEXT("Entomon_ActorInfoTool");
FString UEntomonEditorMode::InteractiveToolName = TEXT("Entomon_MeasureDistanceTool");


UEntomonEditorMode::UEntomonEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UEntomonEditorMode::EM_EntomonEditorModeId,
		LOCTEXT("ModeName", "Entomon"),
		FSlateIcon(),
		true);
}


UEntomonEditorMode::~UEntomonEditorMode()
{
}


void UEntomonEditorMode::ActorSelectionChangeNotify()
{
}

void UEntomonEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FEntomonEditorModeCommands& SampleToolCommands = FEntomonEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UEntomonSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UEntomonInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UEntomonEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FEntomonEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UEntomonEditorMode::GetModeCommands() const
{
	return FEntomonEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
