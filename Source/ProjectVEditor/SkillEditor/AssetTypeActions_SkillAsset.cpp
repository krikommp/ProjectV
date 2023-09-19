#include "AssetTypeActions_SkillAsset.h"

#include "SkillAssetEditorToolkit.h"
#include "..\..\ProjectVGame\Skill\SkillAsset.h"

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
	return USkillAsset::StaticClass();
}

void FAssetTypeActions_SkillAsset::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Asset = Cast<USkillAsset>(*ObjIt);
		if (Asset != NULL)
		{
			TSharedRef< FSkillAssetEditorToolkit > NewToolkit(new FSkillAssetEditorToolkit());
			NewToolkit->Initialize(Mode, EditWithinLevelEditor, Asset);
		}
	}
}

uint32 FAssetTypeActions_SkillAsset::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
