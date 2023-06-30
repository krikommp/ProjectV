// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorWindow.h"

#define LOCTEXT_NAMESPACE "UEditorWindow"

static const FName DefaultTabName("Default");

static const FText DefaultWindowName = LOCTEXT("UEditorWindow", "DefaultWindow");

const FName& UEditorWindow::GetTabName() const
{
	return DefaultTabName;
}

const FText& UEditorWindow::GetDisplayName() const
{
	return DefaultWindowName;
}

void UEditorWindow::ShowWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(GetTabName());
}

TSharedRef<SDockTab> UEditorWindow::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FTestWindowPluginModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("TestWindowPlugin.cpp"))
	);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

#undef LOCTEXT_NAMESPACE

void UTestEditorWindow::ShowWindow()
{
	Super::ShowWindow();
}
