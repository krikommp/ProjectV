#pragma once
#include "SSingleObjectDetailsPanel.h"

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

public:
	void Construct(const FArguments& InArgs, TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor);

	//~ Begin SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;
	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override;
	//~ Begin SSingleObjectDetailsPanel interface
};
