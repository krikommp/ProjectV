// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleTileWidget.h"

#include "SlateOptMacros.h"
#include "Tilemap/TileSet3DAsset.h"
#define LOCTEXT_NAMESPACE "SingleTileWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSingleTileWidget::Construct(const FArguments& InArgs)
{
	TileSetSubObject = InArgs._TileSetSubObject.Get();
	OnTileSetClicked = InArgs._OnTileSetClicked;

	UTexture2D* PreviewTex = TileSetSubObject.BlockTextures[0];
	ensureAlwaysMsgf(PreviewTex, TEXT("Can not get first texture，check config."));

	PreviewBrush = MakeShareable(new FSlateBrush);
	PreviewBrush->SetResourceObject(PreviewTex);
	PreviewBrush->SetImageSize({64, 64});
	PreviewBrush->DrawAs = ESlateBrushDrawType::Image;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SImage)
			.Image(PreviewBrush.Get())
		]
		+ SVerticalBox::Slot()
		  .Padding(0.0f, 10.0f)
		  .AutoHeight()
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnTileSetClicked.ExecuteIfBound(TileSetSubObject.ID);
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text(FText::FromName(TileSetSubObject.ID))
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
