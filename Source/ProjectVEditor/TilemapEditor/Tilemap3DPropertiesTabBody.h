#pragma once
#include "SSingleObjectDetailsPanel.h"
#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TileSet3DAsset.h"
#include "Widget/Tilemap3DEditModeWidget.h"

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
	FTileSet3DCube CurrentTileCube;
	FTileSet3DMesh CurrentTileMesh;
	FGridHeroData CurrentTileChess;
	ETilemap3DEditMode CurrentEditMode = EEM_View;

	//~ Begin Slate UI Components
	TSharedPtr<STextBlock> EditStatusText;
	//~ Begin Slate UI Components

	bool bEditProperty = false;
	int32 CurrentFloor = 0;
public:
	void Construct(const FArguments& InArgs, TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor,
	               TObjectPtr<UTileSet3DAsset> InTileSet);

	//~ Begin SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;
	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override;
	//~ Begin SSingleObjectDetailsPanel interface

	UTilemapAsset* GetTilemapAsset() const { return TilemapEditorPtr.Pin()->TilemapBeingEdited; }
	int32 GetCurrentFloor() const { return CurrentFloor; }
	const FTileSet3DCube& GetTileCube() const { return CurrentTileCube; }
	const FTileSet3DMesh& GetTileMesh() const { return CurrentTileMesh; }
	const FGridHeroData& GetTileChess() const { return CurrentTileChess; }
	UTileSet3DAsset* GetTileSet() const;
	int32 GetTextureIndex(UTexture2D* InTexture) const;
	ETilemap3DEditMode GetEditMode() const { return CurrentEditMode; }
};
