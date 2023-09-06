#include "AssetTypeActions_SkillAsset.h"

#include "Skill/SkillBaseAsset.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FAssetTypeActions_SkillAsset::FAssetTypeActions_SkillAsset(EAssetTypeCategories::Type InAssetCategory)
	:MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_SkillAsset::GetName() const
{
	return LOCTEXT("FAssetTypeActions_SkillAssetName", "Skill Asset");
}

FColor FAssetTypeActions_SkillAsset::GetTypeColor() const
{
	return FColor(30, 44, 255);
}

UClass* FAssetTypeActions_SkillAsset::GetSupportedClass() const
{
	return USkillBaseAsset::StaticClass();
}

void FAssetTypeActions_SkillAsset::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	FAssetTypeActions_Base::OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

uint32 FAssetTypeActions_SkillAsset::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
