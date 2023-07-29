// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapEditor/Tilemap3DEditorManager.h"
#include "Widgets/SCompoundWidget.h"

class UTileSet3DAsset;
/**
 * 
 */
class PROJECTVEDITOR_API STileSetGalleyWidget : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(STileSetGalleyWidget)
			: _TileSet(nullptr), _EditMode(EEM_Cube)
		{
		}

		SLATE_ATTRIBUTE(TWeakObjectPtr<UTileSet3DAsset>, TileSet)
		SLATE_ATTRIBUTE(ETilemap3DEditMode, EditMode)
		SLATE_EVENT(FOnTileSetClicked, OnClicked)
		SLATE_EVENT(FOnClicked, OnFillFloorChlicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
private:
	FOnTileSetClicked OnClicked_Lambda;
	FOnClicked OnFillFloorClicked;
	TSharedPtr<SHorizontalBox> TileCubeBox;
	TSharedPtr<SHorizontalBox> TileMeshBox;
	TWeakObjectPtr<UTileSet3DAsset> TileSet;
	TAttribute<ETilemap3DEditMode> EditMode;	
};
