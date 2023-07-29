// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorWindowManager.h"

#include "EditorWindow/EditorWindow.h"

void UEditorWindowManager::Initialize(FSubsystemCollectionBase& Collection)
{
	RebuildEditorWindows();
}

void UEditorWindowManager::Deinitialize()
{
	for (const auto& TabName : TabNames)
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);
	}
	
	Super::Deinitialize();
}

const UEditorWindowManager& UEditorWindowManager::Get()
{
	return *GEngine->GetEngineSubsystem<UEditorWindowManager>();
}

void UEditorWindowManager::RebuildEditorWindows()
{
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* CurrentClass = *It;
		if (CurrentClass->IsChildOf(UEditorWindow::StaticClass()) && CurrentClass->GetDefaultObject() !=
			UEditorWindow::StaticClass())
		{
			UEditorWindow* Window = Cast<UEditorWindow>(CurrentClass->GetDefaultObject());
			TabNames.Add(Window->GetTabName());
			FGlobalTabmanager::Get()->RegisterNomadTabSpawner(Window->GetTabName(),
			                                                  FOnSpawnTab::CreateUObject(
				                                                  Window, &UEditorWindow::OnSpawnPluginTab))
			                        .SetDisplayName(Window->GetDisplayName())
			                        .SetMenuType(ETabSpawnerMenuType::Hidden);
		}
	}
}
