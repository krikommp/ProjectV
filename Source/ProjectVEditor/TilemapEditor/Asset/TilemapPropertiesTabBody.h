#pragma once
#include "SSingleObjectDetailsPanel.h"

class FTilemapEditorToolkit;

class STilemapPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(STilemapPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	TWeakPtr<FTilemapEditorToolkit> TilemapEditorPtr;

public:
	void Construct(const FArguments& InArgs, TSharedPtr<FTilemapEditorToolkit> InTilemapEditor);

	//~ Begin SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;
	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override;
	//~ Begin SSingleObjectDetailsPanel interface
};
