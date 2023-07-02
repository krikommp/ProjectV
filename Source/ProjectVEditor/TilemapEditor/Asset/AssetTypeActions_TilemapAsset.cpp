#include "AssetTypeActions_TilemapAsset.h"

#include "Tilemap/TilemapAsset.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FAssetTypeActions_TilemapAsset::FAssetTypeActions_TilemapAsset(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_TilemapAsset::GetName() const
{
	return LOCTEXT("FAssetTypeActions_TilemapAssetName", "3D Tilemap");
}

FColor FAssetTypeActions_TilemapAsset::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_TilemapAsset::GetSupportedClass() const
{
	return UTilemapAsset::StaticClass();
}

void FAssetTypeActions_TilemapAsset::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

uint32 FAssetTypeActions_TilemapAsset::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE