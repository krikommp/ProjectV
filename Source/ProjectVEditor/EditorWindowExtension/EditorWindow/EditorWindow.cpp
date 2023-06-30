// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorWindow.h"

#include "EditorWindowViewport.h"

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

TSharedRef<SEditorWindowViewport> UEditorWindow::CreateWindow()
{
	return SNew(SEditorWindowViewport);
}

TSharedRef<SDockTab> UEditorWindow::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			CreateWindow()
		];
}

#undef LOCTEXT_NAMESPACE