// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSetGalleyWidget.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "SingleTileChessWidget.h"
#include "SingleTileWidget.h"
#include "SlateOptMacros.h"
#include "SingleTileMeshWidget.h"
#include "Tilemap/TileSet3DAsset.h"
#define LOCTEXT_NAMESPACE "STileSetGalleyWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STileSetGalleyWidget::Construct(const FArguments& InArgs)
{
	OnClicked_Lambda = InArgs._OnClicked;
	TileSet = InArgs._TileSet.Get();
	EditMode = InArgs._EditMode;
	OnFillFloorClicked = InArgs._OnFillFloorChlicked;

	SAssignNew(TileCubeBox, SHorizontalBox);
	SAssignNew(TileMeshBox, SHorizontalBox);
	SAssignNew(TileChessBox, SHorizontalBox);

	// get the first texture in texture array
	for (const FTileSet3DCube& Tile : TileSet->TileCubeSets)
	{
		TileCubeBox->AddSlot()
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
		TileMeshBox->AddSlot()
		           .AutoWidth()
		           .Padding(10.0f)
		[
			SNew(SSingleTileMeshWidget, Tile)
			.OnTileSetClicked_Lambda([this](const FName& ID)
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnClicked_Lambda.ExecuteIfBound(ID);
			})
		];
	}

	// chess thumbnail
	for (auto [ID, HeroData] : TileSet->ChessMap)
	{
		TileChessBox->AddSlot()
		            .AutoWidth()
		            .Padding(10.0f)
		[
			SNew(SSingleTileChessWidget, HeroData)
			.OnTileSetClicked_Lambda([this](const FName& ID)
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnClicked_Lambda.ExecuteIfBound(ID);
			})
		];
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return EditMode.Get() >= EEM_Cube && EditMode.Get() < EEM_Mesh
					                    ? EVisibility::Visible
					                    : EVisibility::Collapsed;
			             })
			[
				TileCubeBox.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return EditMode.Get() >= EEM_Cube && EditMode.Get() < EEM_Cube_End
					                    ? EVisibility::Visible
					                    : EVisibility::Collapsed;
			             })
			[
				SNew(SButton)
				.OnClicked_Lambda([this]()
				{
					// ReSharper disable once CppExpressionWithoutSideEffects
					OnFillFloorClicked.Execute();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(LOCTEXT("STileSetGalleyWidget", "Fill all this floor."))
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return EditMode.Get() >= EEM_Mesh && EditMode.Get() < EEM_Mesh_End
					                    ? EVisibility::Visible
					                    : EVisibility::Collapsed;
			             })
			[
				TileMeshBox.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return EditMode.Get() == EEM_Chess_Spawn
					                    ? EVisibility::Visible
					                    : EVisibility::Collapsed;
			             })
			[
				TileChessBox.ToSharedRef()
			]
		]
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
