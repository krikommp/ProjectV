#include "AssetTypeActions_Tilemap3DAsset.h"

#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FAssetTypeActions_Tilemap3DAsset::FAssetTypeActions_Tilemap3DAsset(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_Tilemap3DAsset::GetName() const
{
	return LOCTEXT("FAssetTypeActions_TilemapAssetName", "3D Tilemap");
}

FColor FAssetTypeActions_Tilemap3DAsset::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_Tilemap3DAsset::GetSupportedClass() const
{
	return UTilemapAsset::StaticClass();
}

void FAssetTypeActions_Tilemap3DAsset::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Asset = Cast<UTilemapAsset>(*ObjIt);
		if (Asset != NULL)
		{
			TSharedRef< FTilemap3DEditorToolkit > NewToolkit(new FTilemap3DEditorToolkit());
			NewToolkit->Initialize(Mode, EditWithinLevelEditor, Asset);
		}
	}
}

uint32 FAssetTypeActions_Tilemap3DAsset::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE