// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorManager.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_SkillAsset.h"
#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "USkillAssetEditorManager"

void USkillAssetEditorManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 注册菜单
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	// 定义资产分类名
	const EAssetTypeCategories::Type AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Skill")), FText::FromName(TEXT("Skill")));
	ActionType = MakeShareable(new FAssetTypeActions_SkillAsset(AssetCategory));
	AssetTools.RegisterAssetTypeActions(ActionType.ToSharedRef());
}

void USkillAssetEditorManager::Deinitialize()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::Get().GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(ActionType.ToSharedRef());
	}
	Super::Deinitialize();
}

#undef LOCTEXT_NAMESPACE
