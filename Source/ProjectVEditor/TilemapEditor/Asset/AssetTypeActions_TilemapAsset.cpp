#include "AssetTypeActions_TilemapAsset.h"

#include "TilemapEditorToolkit.h"
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
	EToolkitMode::Type Mode = EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Asset = Cast<UTilemapAsset>(*ObjIt);
		if (Asset != NULL)
		{
			TSharedRef< FTilemapEditorToolkit > NewToolkit(new FTilemapEditorToolkit());
			NewToolkit->Initialize(Mode, EditWithinLevelEditor, Asset);
		}
	}
}

uint32 FAssetTypeActions_TilemapAsset::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE