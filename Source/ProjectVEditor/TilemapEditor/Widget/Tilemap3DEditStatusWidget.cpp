// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DEditStatusWidget.h"

#include "SlateOptMacros.h"
#define LOCTEXT_NAMESPACE "STilemap3DEditStatusWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STilemap3DEditStatusWidget::Construct(const FArguments& InArgs, bool InDefaultEditStatus)
{
	bEdit = InDefaultEditStatus;
	OnEditStatusChanged = InArgs._OnEditStatusChanged;

	ChildSlot
	[
		SNew(SButton)
		.OnClicked_Lambda([this]()
		{
			bEdit = !bEdit;
			// ReSharper disable once CppExpressionWithoutSideEffects
			OnEditStatusChanged.ExecuteIfBound(bEdit);
			return FReply::Handled();
		})
		[
			SAssignNew(StatusText, STextBlock)
			.Justification(ETextJustify::Center)
			.Text_Lambda([this]()
			{
				return bEdit
					  ? LOCTEXT("EditStatus_Stop", "Stop Edit")
					  : LOCTEXT("EditStatus_Start", "Start Edit");
			})
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
