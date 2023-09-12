// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillNotesSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillNotesSection"

void SSkillNotesSection::Construct(const FArguments& InArgs)
{
	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);

	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillNotesTitleLabel", "Notes"));

	NextVSlot(WidgetVerticalBox)
	[
		SNew(SBox)
		.HeightOverride(200.0f)
		[
			SNew(SMultiLineEditableTextBox)
			.HintText(LOCTEXT("SkillNotesEditableBoxLabel", "Type in skill notes..."))
		]
	];

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.Header"), 6.0f))
		[
			WidgetVerticalBox.ToSharedRef()
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
