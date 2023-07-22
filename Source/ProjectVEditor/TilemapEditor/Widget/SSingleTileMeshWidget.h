// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectVEditor.h"
#include "Tilemap/TileSet3DAsset.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API SSingleTileMeshWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSingleTileMeshWidget)
		{
		}
	SLATE_EVENT(FOnTileSetClicked, OnTileSetClicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const FTileSet3DMesh& InTileSet);

private:
	FTileSet3DMesh TileSet3DMesh;
	FOnTileSetClicked OnTileSetClicked;
	TSharedPtr<FSlateBrush> PreviewBrush;

	UTexture2D* GetThumbnailFromStaticMesh(UStaticMesh* Mesh) const;
};
