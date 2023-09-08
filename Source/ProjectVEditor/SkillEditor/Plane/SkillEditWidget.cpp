// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditWidget.h"

#include "SkillBaseInfoWidget.h"
#include "SkillInfoWidget.h"
#include "SkillLimitWidget.h"
#include "SkillStartupWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEditWidget"

void SSkillEditWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .HAlign(HAlign_Center)
		  .Padding(FMargin(10.0f, 10.0f))
		  .AutoWidth()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillBaseInfoWidget)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillStartupWidget)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillInfoWidget)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillLimitWidget)
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
