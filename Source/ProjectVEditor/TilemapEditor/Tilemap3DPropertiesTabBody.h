#pragma once
#include "SSingleObjectDetailsPanel.h"
#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TileSet3DAsset.h"

class UTilemapAsset;
class FTilemap3DEditorToolkit;

class STilemap3DPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(STilemap3DPropertiesTabBody)
		{
		}

	SLATE_END_ARGS()

private:
	TWeakPtr<FTilemap3DEditorToolkit> TilemapEditorPtr;
	TWeakObjectPtr<UTileSet3DAsset> TileSet;
	FTileSet3DSubObject CurrentTile;

	//~ Begin Slate UI Components
	TSharedPtr<STextBlock> EditStatusText;
	//~ Begin Slate UI Components

	bool bEditProperty = false;

	TAttribute<int32> CurrentFloor = 0;

	//~ Begin Widget
	TSharedRef<SWidget> DrawEditStatusWidget();
	TSharedRef<SWidget> DrawFloorLineWidget();
	//~ Begin Widget 
public:
	void Construct(const FArguments& InArgs, TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor,
	               TObjectPtr<UTileSet3DAsset> InTileSet);

	//~ Begin SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;
	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override;
	//~ Begin SSingleObjectDetailsPanel interface

	UTilemapAsset* GetTilemapAsset() const { return TilemapEditorPtr.Pin()->TilemapBeingEdited; }
	int32 GetCurrentFloor() const { return CurrentFloor.Get(); }
	const FTileSet3DSubObject& GetCurrentTileProperty() const { return CurrentTile; }
	UTileSet3DAsset* GetTileSet() const;
	int32 GetTextureIndex(UTexture2D* InTexture) const;
};
