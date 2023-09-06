// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillStartupWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillStartupWidget"

void SSkillStartupWidget::Construct(const FArguments& InArgs)
{
	TArray<TSharedPtr<FString>> ComboItems;
	ComboItems.Add(MakeShareable(new FString(TEXT("无"))));
	ComboItems.Add(MakeShareable(new FString(TEXT("物理攻击"))));
	ComboItems.Add(MakeShareable(new FString(TEXT("魔法攻击"))));

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
				.Text(LOCTEXT("StartupTitle", "Startup Settings"))
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
					.Text(LOCTEXT("SkillHitType", "Hit Type:"))
				]
				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				[
					SNew(SComboBox<TSharedPtr<FString> >)
			.OptionsSource(&ComboItems)
			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
					                                     {
						                                     return SNew(STextBlock).Text(FText::FromString(*Item));
					                                     })
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
					                                     {
						                                     if (InSelection.IsValid() && ComboBoxTitleBlock.IsValid())
						                                     {
							                                     ComboBoxTitleBlock->SetText(
								                                     FText::FromString(*InSelection));
						                                     }
					                                     })
					[
						SAssignNew(ComboBoxTitleBlock, STextBlock).Text(LOCTEXT("ComboLabel", "Label"))
					]
				]
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
