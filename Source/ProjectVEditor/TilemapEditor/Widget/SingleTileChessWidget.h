// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Heros/GridHeroData.h"
#include "TilemapEditor/Tilemap3DEditorManager.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API SSingleTileChessWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSingleTileChessWidget)
		{
		}
	SLATE_EVENT(FOnTileSetClicked, OnTileSetClicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FGridHeroData& InHeroData);

private:
	FGridHeroData ChessData;
	FOnTileSetClicked OnTileSetClicked;
	TSharedPtr<FSlateBrush> PreviewBrush;

	UTexture2D* GetThumbnailFromUSkeletalMesh(USkeletalMesh* Mesh) const;
};
