// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSetGalleyWidget.h"

#include "SingleTileWidget.h"
#include "SlateOptMacros.h"
#include "Tilemap/TileSet3DAsset.h"
#define LOCTEXT_NAMESPACE "STileSetGalleyWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STileSetGalleyWidget::Construct(const FArguments& InArgs)
{
	OnClicked_Lambda = InArgs._OnClicked;
	TileSet = InArgs._TileSet.Get();
	SAssignNew(TileSetBox, SHorizontalBox);

	// get the first texture in texture array
	for (FTileSet3DSubObject Tile : TileSet->TileSets)
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
