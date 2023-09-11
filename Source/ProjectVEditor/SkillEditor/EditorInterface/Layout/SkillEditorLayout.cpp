// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorLayout.h"

#include "SlateOptMacros.h"
#include "SkillEditor/EditorInterface/Section/SkillCommonSettingSection.h"
#include "SkillEditor/EditorInterface/Section/SkillDamageSection.h"
#include "SkillEditor/EditorInterface/Section/SkillPerformanceSection.h"
#include "SkillEditor/EditorInterface/Section/SkillRequirementSection.h"
#include "SkillEditor/EditorInterface/Section/SkillSpeechSection.h"

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
				SNew(SSkillCommonSettingSection)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillPerformanceSection)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillSpeechSection)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .Padding(0.0f, 10.0f)
			  .VAlign(VAlign_Top)
			[
				SNew(SSkillRequirementSection)
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
				SNew(SSkillDamageSection)
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
