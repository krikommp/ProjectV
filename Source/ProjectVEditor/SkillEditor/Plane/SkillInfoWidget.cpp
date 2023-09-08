// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillInfoWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillInfoWidget"

void SSkillInfoWidget::Construct(const FArguments& InArgs)
{
	VoiceItems.Add(MakeShareable(new FString(LOCTEXT("SkillUserVoiceLabel", "Do somethings...").ToString())));
	VoiceItems.Add(MakeShareable(new FString(LOCTEXT("SkillUserVoiceLabel", "Do somethings...").ToString())));

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.Header"), 6.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Top)
			  .Padding(10, 10)
			  .AutoHeight()
			[
				SNew(STextBlock)
				.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
				.Text(LOCTEXT("SkillMessageTitle", "Message Settings"))
			]
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Top)
			  .Padding(10, 10)
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .VAlign(VAlign_Center)
				  .FillWidth(0.3f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SkillUseNameLabel", "(User name)"))
				]
				+ SHorizontalBox::Slot()
				  .VAlign(VAlign_Center)
				  .FillWidth(0.7f)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("SkillUserSpeakLabel", "Type in user speak..."))
				]
			]
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Top)
			  .Padding(10, 10)
			  .AutoHeight()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SkillVoiceLabel", "Voice:"))
				]
				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				[
					SNew(SComboBox<TSharedPtr<FString> >)
					.OptionsSource(&VoiceItems)
					.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
					                                     {
						                                     return SNew(STextBlock).Text(FText::FromString(*Item));
					                                     })
					.OnSelectionChanged_Lambda(
						                                     [this](TSharedPtr<FString> InSelection,
						                                            ESelectInfo::Type InSelectInfo)
						                                     {
							                                     if (InSelection.IsValid() && VoiceBoxTitleBlock.
								                                     IsValid())
							                                     {
								                                     VoiceBoxTitleBlock->SetText(
									                                     FText::FromString(*InSelection));
							                                     }
						                                     })
					[
						SAssignNew(VoiceBoxTitleBlock, STextBlock).Text(LOCTEXT("SkillVoiceLabel", "None"))
					]
				]
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
