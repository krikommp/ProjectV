// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorLayout.h"

#include "SlateOptMacros.h"
#include "SkillEditor/EditorInterface/Section/SkillCommonSettingSection.h"
#include "SkillEditor/EditorInterface/Section/SkillDamageSection.h"
#include "SkillEditor/EditorInterface/Section/SkillEffectSection.h"
#include "SkillEditor/EditorInterface/Section/SkillNotesSection.h"
#include "SkillEditor/EditorInterface/Section/SkillPerformanceSection.h"
#include "SkillEditor/EditorInterface/Section/SkillRequirementSection.h"
#include "SkillEditor/EditorInterface/Section/SkillSpeechSection.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEditorLayout"

void SSkillEditorLayout::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
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
				+ SVerticalBox::Slot()
				  .AutoHeight()
				  .Padding(0.0f, 10.0f)
				  .VAlign(VAlign_Top)
				[
					SNew(SSkillEffectSection)
				]
				+ SVerticalBox::Slot()
				  .AutoHeight()
				  .Padding(0.0f, 10.0f)
				  .VAlign(VAlign_Top)
				[
					SNew(SSkillNotesSection)
				]
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
