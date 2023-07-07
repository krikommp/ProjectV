#pragma once
#include "SSingleObjectDetailsPanel.h"
#include "Tilemap3DEditorToolkit.h"

class UTilemapAsset;
class FTilemap3DEditorToolkit;

class STilemap3DPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(STilemap3DPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	TWeakPtr<FTilemap3DEditorToolkit> TilemapEditorPtr;

	//~ Begin Slate UI Components
	TSharedPtr<STextBlock> EditStatusText;
	//~ Begin Slate UI Components

	bool bEditProperty = false;

	int32 CurrentFloor = 0;
public:
	void Construct(const FArguments& InArgs, TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor);

	//~ Begin SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;
	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override;
	//~ Begin SSingleObjectDetailsPanel interface

	UTilemapAsset* GetTilemapAsset() const { return TilemapEditorPtr.Pin()->TilemapBeingEdited; }
	int32 GetCurrentFloor() const { return CurrentFloor; } 
};
