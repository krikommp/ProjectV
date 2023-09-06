// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillBaseInfoWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillBaseInfoWidget"

void SSkillBaseInfoWidget::Construct(const FArguments& InArgs)
{
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
				.Text(LOCTEXT("BaseInfoTitle", "Base Settings"))
			]
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Top)
			  .Padding(10, 10)
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Center)
				  .AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillName", "Name:"))
					]
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(SBox).MinDesiredHeight(20.f).MinDesiredWidth(400.f)
						[
							SNew(SEditableTextBox)
							.HintText(LOCTEXT("SkillNameEditBox", "Type in skill name..."))
						]
					]
				]
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Center)
				  .AutoWidth()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					  .HAlign(HAlign_Right)
					  .AutoWidth()
					  .Padding(10.0f, 0.0f)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						  .VAlign(VAlign_Top)
						  .AutoHeight()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("SkillIcon", "Icon:"))
						]
						+ SVerticalBox::Slot()
						  .VAlign(VAlign_Top)
						  .AutoHeight()
						[
							SNew(STextBlock)
							.Font(FAppStyle::Get().GetFontStyle("SmallFont"))
							.Text(LOCTEXT("SkillIconID", "76"))
						]
					]
					+ SHorizontalBox::Slot()
					  .HAlign(HAlign_Center)
					  .AutoWidth()
					  .Padding(10.0f, 0.0f)
					[
						SNew(SBox).MinDesiredHeight(64.f).MinDesiredWidth(64.f)
						[
							SNew(SBorder)
							.BorderImage(
								new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentBlue"),
								                          6.0f))
						]
					]
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
					.Text(LOCTEXT("SkillDescription", "Description:"))
				]
				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				[
					SNew(SBox).MinDesiredHeight(60.f).MinDesiredWidth(600.f)
					[
						SNew(SMultiLineEditableTextBox)
						.HintText(LOCTEXT("SkillDescriptionEditBox", "Type in skill description..."))
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
						.Text(LOCTEXT("SkillAPCost", "AP Cost:"))
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
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .Padding(10.0f, 0.0f)
				  .VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillHPCost", "HP Cost:"))
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
						.Text(LOCTEXT("SkillVisibleRange", "Visible Range:"))
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
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .Padding(10.0f, 0.0f)
				  .VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillEffectRange", "Effect Range:"))
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
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
