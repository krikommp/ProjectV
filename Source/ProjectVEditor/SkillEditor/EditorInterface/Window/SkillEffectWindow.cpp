// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectWindow.h"

#include "SlateOptMacros.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEffectWindow"

namespace SkillEffect_Internal
{
	struct FPercentTypeInterface : public TDefaultNumericTypeInterface<float>
	{
		virtual FString ToString(const float& Value) const override
		{
			static const FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions()
			                                                                .SetUseGrouping(false)
			                                                                .SetMinimumFractionalDigits(0)
			                                                                .SetMaximumFractionalDigits(2);
			FString NumberString = FastDecimalFormat::NumberToString(Value * 100.f,
			                                                         ExpressionParser::GetLocalizedNumberFormattingRules(),
			                                                         NumberFormattingOptions);
			return NumberString + TEXT('%');
		}

		virtual TOptional<float> FromString(const FString& InString, const float& ExistingValue)
		{
			TOptional<float> ParsedValue = TDefaultNumericTypeInterface<float>::FromString(InString, ExistingValue);
			if (ParsedValue.IsSet())
			{
				return ParsedValue.GetValue() / 100.f;
			}
			return ParsedValue;
		}
	};

	SHorizontalBox::FScopedWidgetSlotArguments NextLinkedSlot(TSharedPtr<SHorizontalBox> HorizontalBox)
	{
		HorizontalBox->AddSlot()
		             .AutoWidth()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("LinkedSign", "+"))
		];

		SHorizontalBox::FScopedWidgetSlotArguments NewSlot = HorizontalBox->AddSlot();
		NewSlot.AutoWidth()
		       .Padding(12.0f, 0.0f);
		return MoveTemp(NewSlot);
	}
}

TSharedRef<SWidget> SSkillEffectWindow::CreateRecoverContent()
{
	TSharedRef<SVerticalBox> RecoverContent = SNew(SVerticalBox);

	const TSharedRef<SHorizontalBox> HPLinkedHBox = SNew(SHorizontalBox);
	SkillEffect_Internal::NextLinkedSlot(HPLinkedHBox)
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
			.Delta(1.0f)
		]
	];
	const TSharedRef<SHorizontalBox> MPLinkedHBox = SNew(SHorizontalBox);
	SkillEffect_Internal::NextLinkedSlot(MPLinkedHBox)
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
			.Delta(1.0f)
		]
	];
	NextVCheckSlot(RecoverContent, LOCTEXT("Recover HP Label", "Recover HP"))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.WidthOverride(200.0f)
			[
				SNew(SSpinBox<float>)
				.MinValue(0.0f)
				.MaxValue(1.0f)
				.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
				.MaxSliderValue(TAttribute<TOptional<float>>(1.0f))
				.Value(0)
				.Delta(0.01f)
				.TypeInterface(MakeShareable(new SkillEffect_Internal::FPercentTypeInterface()))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			HPLinkedHBox
		]
	];
	NextVCheckSlot(RecoverContent, LOCTEXT("Recover MP Label", "Recover MP"))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.WidthOverride(200.0f)
			[
				SNew(SSpinBox<float>)
				.MinValue(0.0f)
				.MaxValue(1.0f)
				.MinSliderValue(TAttribute<TOptional<float>>(0.0f))
				.MaxSliderValue(TAttribute<TOptional<float>>(1.0f))
				.Value(0)
				.Delta(0.01f)
				.TypeInterface(MakeShareable(new SkillEffect_Internal::FPercentTypeInterface()))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MPLinkedHBox
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
	if (InArgs._HeaderTitle.IsBound())
	{
		HeaderTitle = InArgs._HeaderTitle;
	}else
	{
		HeaderTitle = ESkillEffectHeaderTitle::Recover;
	}

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
							SNew(SSegmentedControl<ESkillEffectHeaderTitle>)
							.Value(HeaderTitle)
							.OnValueChanged_Lambda([this](ESkillEffectHeaderTitle NewTitle) { HeaderTitle = NewTitle; })
							+ SSegmentedControl<ESkillEffectHeaderTitle>::Slot(ESkillEffectHeaderTitle::Recover)
							.Text(LOCTEXT("Skill Recover Label", "Recover"))
							+ SSegmentedControl<ESkillEffectHeaderTitle>::Slot(ESkillEffectHeaderTitle::State)
							.Text(LOCTEXT("Skill State Label", "State"))
							+ SSegmentedControl<ESkillEffectHeaderTitle>::Slot(ESkillEffectHeaderTitle::Param)
							.Text(LOCTEXT("Skill Param Label", "Param"))
							+ SSegmentedControl<ESkillEffectHeaderTitle>::Slot(ESkillEffectHeaderTitle::Other)
							.Text(LOCTEXT("Skill Other Label", "Other"))
						]
					]
					+ SVerticalBox::Slot()
					.FillHeight(0.7f)
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						.Visibility_Lambda([this]() { return HeaderTitle.Get() == ESkillEffectHeaderTitle::Recover ? EVisibility::Visible : EVisibility::Collapsed; })
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
