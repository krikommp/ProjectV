// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSetGalleyWidget.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "SingleTileWidget.h"
#include "SlateOptMacros.h"
#include "SSingleTileMeshWidget.h"
#include "Tilemap/TileSet3DAsset.h"
#define LOCTEXT_NAMESPACE "STileSetGalleyWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STileSetGalleyWidget::Construct(const FArguments& InArgs)
{
	OnClicked_Lambda = InArgs._OnClicked;
	TileSet = InArgs._TileSet.Get();
	SAssignNew(TileSetBox, SHorizontalBox);

	// get the first texture in texture array
	for (const FTileSet3DCube& Tile : TileSet->TileCubeSets)
	{
		TileSetBox->AddSlot()
		          .AutoWidth()
		          .Padding(10.0f)
		[
			SNew(SSingleTileWidget)
			.TileSetSubObject(Tile)
			.OnTileSetClicked_Lambda([this](const FName& ID)
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnClicked_Lambda.ExecuteIfBound(ID);
			})
		];
	}

	// get static mesh's thumbnail
	for (const FTileSet3DMesh& Tile : TileSet->TileMeshSets)
	{
		//UStaticMesh* Mesh = Tile.Mesh;
		TileSetBox->AddSlot()
				  .AutoWidth()
				  .Padding(10.0f)
		[
			SNew(SSingleTileMeshWidget, Tile)
		];
	}

	ChildSlot
	[
		TileSetBox.ToSharedRef()
	];
}

// FReply STileSetGalleyWidget::HandleButtonClick()
// {
// 	if (OnClicked_Lambda.IsBound())
// 	{
// 		OnClicked_Lambda.Execute(Index);
// 	}
// 	return FReply::Handled();
// }

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
