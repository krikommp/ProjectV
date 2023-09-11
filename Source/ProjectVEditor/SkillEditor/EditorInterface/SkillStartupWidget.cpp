// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillStartupWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SSpinBox.h"
#include "Layout/WidgetLayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillStartupWidget"

void SSkillStartupWidget::Construct(const FArguments& InArgs)
{
	ComboItems.Add(MakeShareable(new FString(LOCTEXT("SkillTypeTabel", "Magic Skill").ToString())));
	ComboItems.Add(MakeShareable(new FString(LOCTEXT("SkillTypeTabel", "Physical Skill").ToString())));

	AnimationItems.Add(MakeShareable(new FString(LOCTEXT("SkillAnimLabel", "Melee").ToString())));
	AnimationItems.Add(MakeShareable(new FString(LOCTEXT("SkillAnimLabel", "Defense").ToString())));

	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);
	TSharedRef<SHorizontalBox> SkillPointHBox = SNew(SHorizontalBox);
	TSharedRef<SHorizontalBox> SkillAssetHBox = SNew(SHorizontalBox);

	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillInfoTitle", "Skill Info"));
	NextHSlot(SkillPointHBox, LOCTEXT("SkillHitRate", "Hit Rate:"))
	[
		SNew(SSpinBox<float>)
					.MinValue(0.0f)
					.MaxValue(100.0f)
					.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
					.MaxSliderValue(TAttribute<TOptional<float>>(100.0f))
					.Value(0)
					.Delta(0.5f)
	];
	NextHSlot(SkillPointHBox, LOCTEXT("SkillTechniquePoints", "TP:"))
	[
		SNew(SSpinBox<float>)
					.MinValue(0.0f)
					.MaxValue(500.0f)
					.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
					.MaxSliderValue(TAttribute<TOptional<float>>(500.0f))
					.Value(0)
					.Delta(0.5f)
	];
	NextHSlot(SkillAssetHBox, LOCTEXT("SkillHitType", "Hit Type:"))
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
				                                     ComboBoxTitleBlock->SetText(FText::FromString(*InSelection));
			                                     }
		                                     })
		[
			SAssignNew(ComboBoxTitleBlock, STextBlock).Text(LOCTEXT("SkillTypeLabel", "None"))
		]
	];
	NextHSlot(SkillAssetHBox, LOCTEXT("SkillAnimAsset", "Animation:"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
			.OptionsSource(&AnimationItems)
			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		                                     {
			                                     return SNew(STextBlock).Text(FText::FromString(*Item));
		                                     })
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
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
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillPointHBox
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillAssetHBox
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
