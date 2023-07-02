// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapEditorManager.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Asset/AssetTypeActions_TilemapAsset.h"

#define LOCTEXT_NAMESPACE "UTilemapEditorManager"

void UTilemapEditorManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	// 定义资产分类名
	const EAssetTypeCategories::Type AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Tilemap")), FText::FromName(TEXT("Tilemap")));
	ActionType = MakeShareable(new FAssetTypeActions_TilemapAsset(AssetCategory));
	AssetTools.RegisterAssetTypeActions(ActionType.ToSharedRef());
}

void UTilemapEditorManager::Deinitialize()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::Get().GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(ActionType.ToSharedRef());
	}
	
	Super::Deinitialize();
}

#undef LOCTEXT_NAMESPACE
