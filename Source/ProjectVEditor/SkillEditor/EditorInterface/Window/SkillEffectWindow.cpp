// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectWindow.h"

#include "SlateOptMacros.h"
#include "Math/UnitConversion.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEffectWindow"

TSharedRef<SWidget> SSkillEffectWindow::CreateRecoverContent()
{
	TSharedRef<SVerticalBox> RecoverContent = SNew(SVerticalBox);

	NextVCheckSlot(RecoverContent, LOCTEXT("Recover HP Label", "Recover HP"))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.WidthOverride(200.0f)
			[
				SNew(SSpinBox<double>)
				.MinValue(0.0f)
				.MaxValue(100.0f)
				.MinSliderValue(TAttribute<TOptional<double>>(0.0f))
				.MaxSliderValue(TAttribute<TOptional<double>>(100.0f))
				.Value(0)
				.Delta(0.5f)
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LinkedSign", "+"))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(200.0f)
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
		]

	];
	NextVCheckSlot(RecoverContent, LOCTEXT("Recover MP Label", "Recover MP"))
	[
		SNew(SBox)
		.WidthOverride(200.0f)
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
	NextVCheckSlot(RecoverContent, LOCTEXT("Recover TP Label", "Gain TP"))
	[
		SNew(SBox)
		.WidthOverride(200.0f)
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

	return RecoverContent;
}

void SSkillEffectWindow::Construct(const FArguments& InArgs)
{
	ParentWindowPtr = InArgs._ParentWindow.Get();

	SWindow::Construct(SWindow::FArguments()
	                   .Title(LOCTEXT("SkillEffectDetailEditorLabel", "SkillEffectDetailEditor"))
	                   .SupportsMinimize(false)
	                   .SupportsMaximize(false)
	                   .SizingRule(ESizingRule::FixedSize)
	                   .AutoCenter(EAutoCenter::PreferredWorkArea)
	                   .ClientSize(FVector2D(450, 450))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(2)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							SNew(SSegmentedControl<ESkillEffectWindowType>)
							+ SSegmentedControl<ESkillEffectWindowType>::Slot(ESkillEffectWindowType::Recover)
							.Text(LOCTEXT("Skill Recover Label", "Recover"))
							+ SSegmentedControl<ESkillEffectWindowType>::Slot(ESkillEffectWindowType::State)
							.Text(LOCTEXT("Skill State Label", "State"))
							+ SSegmentedControl<ESkillEffectWindowType>::Slot(ESkillEffectWindowType::Param)
							.Text(LOCTEXT("Skill Param Label", "Param"))
							+ SSegmentedControl<ESkillEffectWindowType>::Slot(ESkillEffectWindowType::Other)
							.Text(LOCTEXT("Skill Other Label", "Other"))
						]
					]
					+ SVerticalBox::Slot()
					.FillHeight(0.7f)
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						[
							CreateRecoverContent()
						]
					]
				]
			]
		]);
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
