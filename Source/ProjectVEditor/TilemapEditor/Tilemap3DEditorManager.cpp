// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DEditorManager.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"
#include "Tilemap3DEditorSettings.h"
#include "AssetTypeActions_Tilemap3DAsset.h"

FTilemap3DEditDelegates::FOnTilemapEditStatueChanged FTilemap3DEditDelegates::OnTilemapEditStatueChanged;
FSimpleMulticastDelegate FTilemap3DEditDelegates::OnTilemapModelChanged;

#define LOCTEXT_NAMESPACE "UTilemapEditorManager"

void UTilemap3DEditorManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 注册菜单
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	// 定义资产分类名
	const EAssetTypeCategories::Type AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Tilemap")), FText::FromName(TEXT("Tilemap")));
	ActionType = MakeShareable(new FAssetTypeActions_Tilemap3DAsset(AssetCategory));
	AssetTools.RegisterAssetTypeActions(ActionType.ToSharedRef());

	// 注册配置菜单
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Project", "TilemapEditor",
			LOCTEXT("TilemapSettingsName", "Tilemap Editor"),
			LOCTEXT("TilemapSettingsDesc", "Setting for tilemap"),
			GetMutableDefault<UTilemap3DEditorSettings>());
	}
#endif
}

void UTilemap3DEditorManager::Deinitialize()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::Get().GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(ActionType.ToSharedRef());
	}

#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Project", "TilemapEditor");
	}
#endif
	
	
	Super::Deinitialize();
}

#undef LOCTEXT_NAMESPACE
