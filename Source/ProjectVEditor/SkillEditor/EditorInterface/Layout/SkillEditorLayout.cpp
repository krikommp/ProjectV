// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorLayout.h"

#include "SkillEditor/EditorInterface/SkillBaseInfoWidget.h"
#include "SkillEditor/EditorInterface/SkillDamageWidget.h"
#include "SkillEditor/EditorInterface/SkillInfoWidget.h"
#include "SkillEditor/EditorInterface/SkillLimitWidget.h"
#include "SkillEditor/EditorInterface/SkillStartupWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEditorLayout"

void SSkillEditorLayout::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .HAlign(HAlign_Fill)
		  .Padding(FMargin(10.0f, 10.0f))
		  .FillWidth(1.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
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
		+ SHorizontalBox::Slot()
		  .HAlign(HAlign_Fill)
		  .Padding(FMargin(10.0f, 10.0f))
		  .FillWidth(1.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillDamageWidget)
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
