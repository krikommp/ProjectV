// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillCommonSettingSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillCommonSettingSection"
void SSkillCommonSettingSection::Construct(const FArguments& InArgs)
{
	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);
	TSharedPtr<SHorizontalBox> SkillInfoHBox_1 = SNew(SHorizontalBox);
	TSharedPtr<SHorizontalBox> SkillInfoHBox_2 = SNew(SHorizontalBox);
	TSharedPtr<SHorizontalBox> SkillInfoHBox_3 = SNew(SHorizontalBox);

	TitleSlot(WidgetVerticalBox, LOCTEXT("BaseInfoTitle", "Base Settings"));
	NextHSlot(SkillInfoHBox_1, LOCTEXT("SkillName", "Name:"))
	[
		SNew(SBox)
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("SkillNameEditBox", "Type in skill name..."))
		]
	];
	NextHSlot(SkillInfoHBox_1, LOCTEXT("SkillIcon", "Icon:"))
	[
		SNew(SBox)
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("SkillIconEditBox", "Type in skill icon..."))
		]
	];
	NextHSlot(SkillInfoHBox_1, FText::GetEmpty(), HAlign_Left)
	[
		SNew(SBox).HeightOverride(64.f).WidthOverride(64.f)
		[
			SNew(SBorder)
			.BorderImage(
				new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentBlue"),
				                          6.0f))
		]
	];
	NextHSlot(SkillInfoHBox_2, LOCTEXT("SkillAPCost", "AP Cost:"))
	[
		SNew(SBox)
		[
			SNew(SSpinBox<float>)
				.MinValue(0.0f)
				.MaxValue(500.0f)
				.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
				.MaxSliderValue(TAttribute<TOptional<float>>(500.0f))
				.Value(0)
				.Delta(0.5f)
		]
	];
	NextHSlot(SkillInfoHBox_2, LOCTEXT("SkillHPCost", "HP Cost:"))
	[
		SNew(SBox)
		[
			SNew(SSpinBox<float>)
				.MinValue(0.0f)
				.MaxValue(500.0f)
				.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
				.MaxSliderValue(TAttribute<TOptional<float>>(500.0f))
				.Value(0)
				.Delta(0.5f)
		]
	];
	NextHSlot(SkillInfoHBox_3, LOCTEXT("SkillVisibleRange", "Visible Range:"))
	[
		SNew(SBox)
		[
			SNew(SSpinBox<float>)
			.MinValue(0.0f)
			.MaxValue(500.0f)
			.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
			.MaxSliderValue(TAttribute<TOptional<float>>(500.0f))
			.Value(0)
			.Delta(0.5f)
		]
	];
	NextHSlot(SkillInfoHBox_3, LOCTEXT("SkillEffectRange", "Effect Range:"))
	[
		SNew(SBox)
		[
			SNew(SSpinBox<float>)
				.MinValue(0.0f)
				.MaxValue(500.0f)
				.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
				.MaxSliderValue(TAttribute<TOptional<float>>(500.0f))
				.Value(0)
				.Delta(0.5f)
		]
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillInfoHBox_1.ToSharedRef()
	];
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillDescription", "Description:"))
	[
		SNew(SBox)
		[
			SNew(SMultiLineEditableTextBox)
			.HintText(LOCTEXT("SkillDescriptionEditBox", "Type in skill description..."))
		]
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillInfoHBox_2.ToSharedRef()
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillInfoHBox_3.ToSharedRef()
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
