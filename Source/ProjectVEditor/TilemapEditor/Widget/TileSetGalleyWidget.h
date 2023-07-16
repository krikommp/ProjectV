// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectVEditor.h"
#include "Widgets/SCompoundWidget.h"

class UTileSet3DAsset;
/**
 * 
 */
class PROJECTVEDITOR_API STileSetGalleyWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STileSetGalleyWidget)
			: _TileSet(nullptr)
		{
		}

		SLATE_ATTRIBUTE(TWeakObjectPtr<UTileSet3DAsset>, TileSet)
		SLATE_EVENT(FOnTileSetClicked, OnClicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
private:
	FOnTileSetClicked OnClicked_Lambda;
	TSharedPtr<SHorizontalBox> TileSetBox;
	TWeakObjectPtr<UTileSet3DAsset> TileSet;
};
