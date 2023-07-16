// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectVEditor.h"
#include "Tilemap/TileSet3DAsset.h"
#include "Widgets/SCompoundWidget.h"

struct FTileSet3DSubObject;
/**
 * 
 */
class PROJECTVEDITOR_API SSingleTileWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSingleTileWidget)
		{
		}

		SLATE_ATTRIBUTE(FTileSet3DSubObject, TileSetSubObject)
		SLATE_EVENT(FOnTileSetClicked, OnTileSetClicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	FTileSet3DSubObject TileSetSubObject;
	FOnTileSetClicked OnTileSetClicked;
	TSharedPtr<FSlateBrush> PreviewBrush;
};
