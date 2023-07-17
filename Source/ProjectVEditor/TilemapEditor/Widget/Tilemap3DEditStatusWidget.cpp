// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DEditStatusWidget.h"

#include "SlateOptMacros.h"
#define LOCTEXT_NAMESPACE "STilemap3DEditStatusWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STilemap3DEditStatusWidget::Construct(const FArguments& InArgs, bool InDefaultEditStatus)
{
	bEdit = InDefaultEditStatus;
	
	ChildSlot
	[
		SNew(SButton)
		.OnClicked_Lambda([this]()
		{
			bEdit = !bEdit;
		})
		[
			SAssignNew(StatusText, STextBlock)
			.Justification(ETextJustify::Center)
			.Text(bEdit ? LOCTEXT("STilemap3DEditStatusWidget", "Stop Edit") : LOCTEXT("STilemap3DEditStatusWidget", "Start Edit"))
		]
	];
	
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE