// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DFloorStatusWidget.h"

#include "SlateOptMacros.h"
#define LOCTEXT_NAMESPACE "STilemap3DEditStatusWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STilemap3DFloorStatusWidget::Construct(const FArguments& InArgs)
{
	Floor = InArgs._Floor;
	OnFloorIncrease = InArgs._OnFloorIncrease;
	OnFloorDecrease = InArgs._OnFloorDecrease;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .HAlign(HAlign_Left)
		  .Padding(FMargin(6.0f, 2.0f))
		[
			SNew(STextBlock)
			.Justification(ETextJustify::Center)
			.Text_Lambda([this]()
			                {
				                return FText::Format(LOCTEXT("TilemapFloorEntryLabel", "Floor: {0}"), Floor.Get());
			                })
		]
		+ SHorizontalBox::Slot()
		  .FillWidth(1.0f)
		  .HAlign(HAlign_Right)
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnFloorDecrease.ExecuteIfBound(Floor.Get());
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text(LOCTEXT("STiemap3DFloorStatusWidget", "<"))
			]
		]
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .HAlign(HAlign_Right)
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnFloorIncrease.ExecuteIfBound(Floor.Get());
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
			.Justification(ETextJustify::Center)
			.Text(LOCTEXT("STiemap3DFloorStatusWidget", ">"))
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
