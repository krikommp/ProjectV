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

	auto TitleSlot = [](TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel)
	{
		VerticalBox->AddSlot()
		           .VAlign(VAlign_Top)
		           .Padding(10.0f, 10.0f)
		           .AutoHeight()
		[
			SNew(STextBlock)
			.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
			.Text(InLabel)
		];
	};

	auto NextVSlot = [](TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel)
	{
		TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);

		VerticalBox->AddSlot()
		           .VAlign(VAlign_Top)
		           .Padding(10.0f, 10.0f)
		           .AutoHeight()
		[
			VBox
		];

		VBox->AddSlot()
		    .VAlign(VAlign_Top)
		    .AutoHeight()
		[
			SNew(STextBlock)
			.Text(InLabel)
		];

		SVerticalBox::FScopedWidgetSlotArguments NewSlot = VBox->AddSlot();
		NewSlot.VAlign(VAlign_Top)
		       .AutoHeight();
		return MoveTemp(NewSlot);
	};

	auto NextHSlot = [](TSharedPtr<SHorizontalBox> HorizontalBox, const FText& InLabel)
	{
		TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);
		
		HorizontalBox->AddSlot()
		.HAlign(HAlign_Fill)
		.FillWidth(1.0f)
		[
			VBox
		];

		VBox->AddSlot()
			.VAlign(VAlign_Top)
			.AutoHeight()
		[
			SNew(STextBlock)
			.Text(InLabel)
		];

		SVerticalBox::FScopedWidgetSlotArguments NewSlot = VBox->AddSlot();
		NewSlot.VAlign(VAlign_Top)
			   .AutoHeight();
		return MoveTemp(NewSlot);
	};
	
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
