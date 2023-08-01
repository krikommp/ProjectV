#pragma once
#include "SSingleObjectDetailsPanel.h"
#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TileSet3DAsset.h"
#include "Widget/Tilemap3DEditModeWidget.h"

class STilemap3DChessDataDetails;
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
	ETilemap3DEditMode CurrentEditMode = EEM_View;
	FName TileSetID = FName();

	//~ Begin Slate UI Components
	TSharedPtr<STextBlock> EditStatusText;
	TSharedPtr<STilemap3DChessDataDetails> ChessDataDetails;
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
	const FTileSet3DCube* GetTileCube() const;
	const FTileSet3DMesh* GetTileMesh() const;
	const FGridHeroData* GetTileChess() const;
	UTileSet3DAsset* GetTileSet() const;
	int32 GetTextureIndex(UTexture2D* InTexture) const;
	ETilemap3DEditMode GetEditMode() const { return CurrentEditMode; }
	TSharedPtr<STilemap3DChessDataDetails> GetChessDataDetails() const { return ChessDataDetails; }
	
	UGridChessData* SelectedChessData = nullptr;
};
