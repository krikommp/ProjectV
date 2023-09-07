// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillStartupWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillStartupWidget"

void SSkillStartupWidget::Construct(const FArguments& InArgs)
{
	ComboItems.Add(MakeShareable(new FString(LOCTEXT("SkillTypeTabel", "Magic Skill").ToString())));
	ComboItems.Add(MakeShareable(new FString(LOCTEXT("SkillTypeTabel", "Physical Skill").ToString())));

	AnimationItems.Add(MakeShareable(new FString(LOCTEXT("SkillAnimLabel", "Melee").ToString())));
	AnimationItems.Add(MakeShareable(new FString(LOCTEXT("SkillAnimLabel", "Defense").ToString())));

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
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillHitRate", "Hit Rate:"))
					]
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(SSpinBox<float>)
						.MinValue(0.0f)
						.MaxValue(100.0f)
						.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
						.MaxSliderValue(TAttribute<TOptional<float>>(100.0f))
						.Value(0)
						.Delta(0.5f)
					]
				]
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillTechniquePoints", "TP:"))
					]
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(SSpinBox<float>)
						.MinValue(0.0f)
						.MaxValue(500.0f)
						.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
						.MaxSliderValue(TAttribute<TOptional<float>>(500.0f))
						.Value(0)
						.Delta(0.5f)
					]
				]
			]
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Top)
			  .Padding(10, 10)
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .VAlign(VAlign_Center)
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
				.OnSelectionChanged_Lambda(
							                                     [this](TSharedPtr<FString> InSelection,
							                                            ESelectInfo::Type InSelectInfo)
							                                     {
								                                     if (InSelection.IsValid() && ComboBoxTitleBlock.
									                                     IsValid())
								                                     {
									                                     ComboBoxTitleBlock->SetText(
										                                     FText::FromString(*InSelection));
								                                     }
							                                     })
						[
							SAssignNew(ComboBoxTitleBlock, STextBlock).Text(LOCTEXT("SkillTypeLabel", "None"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .VAlign(VAlign_Center)
				  .Padding(10.0f, 0.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillAnimAsset", "Animation:"))
					]
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(SComboBox<TSharedPtr<FString> >)
				.OptionsSource(&AnimationItems)
				.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
						                                     {
							                                     return SNew(STextBlock).Text(FText::FromString(*Item));
						                                     })
				.OnSelectionChanged_Lambda(
							                                     [this](TSharedPtr<FString> InSelection,
							                                            ESelectInfo::Type InSelectInfo)
							                                     {
								                                     if (InSelection.IsValid() && AnimBoxTitleBlock.
									                                     IsValid())
								                                     {
									                                     AnimBoxTitleBlock->SetText(
										                                     FText::FromString(*InSelection));
								                                     }
							                                     })
						[
							SAssignNew(AnimBoxTitleBlock, STextBlock).Text(LOCTEXT("SkillAnimLabel", "None"))
						]
					]
				]
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
