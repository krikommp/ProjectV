// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSpeechSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SSpinBox.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillSpeechSection"

void SSkillSpeechSection::Construct(const FArguments& InArgs)
{
	VoiceItems.Add(MakeShareable(new FString(LOCTEXT("SkillUserVoiceLabel", "Do somethings...").ToString())));
	VoiceItems.Add(MakeShareable(new FString(LOCTEXT("SkillUserVoiceLabel", "Do somethings...").ToString())));

	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);

	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillMessageTitle", "Message Settings"));
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillUseNameLabel", "(User name)"))
	[
		SNew(SEditableTextBox)
		.HintText(LOCTEXT("SkillUserSpeakLabel", "Type in user speak..."))
	];
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillVoiceLabel", "Voice:"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&VoiceItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		                                     {
			                                     return SNew(STextBlock).Text(FText::FromString(*Item));
		                                     })
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		                                     {
			                                     if (InSelection.IsValid() && VoiceBoxTitleBlock.IsValid())
			                                     {
				                                     VoiceBoxTitleBlock->SetText(FText::FromString(*InSelection));
			                                     }
		                                     })
		[
			SAssignNew(VoiceBoxTitleBlock, STextBlock).Text(LOCTEXT("SkillVoiceLabel", "None"))
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
