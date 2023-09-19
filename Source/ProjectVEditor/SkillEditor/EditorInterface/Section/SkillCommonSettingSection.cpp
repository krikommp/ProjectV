// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillCommonSettingSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "..\..\..\..\ProjectVGame\Skill\SkillAsset.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"
#include "SkillEditor/EditorInterface/Window/SkillIconWindow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillCommonSettingSection"

void SSkillCommonSettingSection::Construct(const FArguments& InArgs, TWeakObjectPtr<USkillAsset> InSkillAsset)
{
	SkillAsset = InSkillAsset;
	
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
			.Text_Lambda([this](){ return SkillAsset->Name; })
			.OnTextChanged_Lambda([this](const FText& NewText){ SkillAsset->Name = NewText; })
		]
	];
	NextHSlot(SkillInfoHBox_1, LOCTEXT("SkillIcon", "Icon:"))
	[
		SNew(SBox)
		[
			SNew(SEditableTextBox)
			.IsEnabled(false)
			.HintText(LOCTEXT("SkillIconEditBox", "Type in skill icon..."))
			.Text_Lambda([this](){ return SkillAsset->Icon != nullptr ? FText::FromString(SkillAsset->Icon->GetName()) : FText::GetEmpty(); })
		]
	];
	NextHSlot(SkillInfoHBox_1, FText::GetEmpty(), HAlign_Left)
	[
		SNew(SBox).HeightOverride(96.f).WidthOverride(96.f)
		[
			SNew(SBorder)
			.BorderImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.Panel"), 6.0f))
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Lambda([=]()
				{
					using WindowType = SSkillIconWindow;
					TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
					TSharedPtr<WindowType> Window = SNew(WindowType, SkillAsset);
					if (RootWindow.IsValid())
					{
						FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), RootWindow.ToSharedRef());
					}
					else
					{
						FSlateApplication::Get().AddWindow(Window.ToSharedRef());
					}
					return FReply::Handled();
				})
				.Content()
				[
					SNew(SImage)
					.Image_Lambda([this]()
					{
						if (SkillAsset->Icon == nullptr)
							return FCoreStyle::Get().GetBrush("Checkerboard");
						else
						{
							FSlateBrush* PreviewBrush = new FSlateBrush;
							PreviewBrush->SetResourceObject(SkillAsset->Icon);
							PreviewBrush->DrawAs = ESlateBrushDrawType::Image;
							return const_cast<const FSlateBrush*>(PreviewBrush);
						}
					})
				]
			]
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
		.HeightOverride(100.0f)
		[
			SNew(SMultiLineEditableTextBox)
			.HintText(LOCTEXT("SkillDescriptionEditBox", "Type in skill description..."))
			.Text_Lambda([this]() { return SkillAsset->Description; })
			.OnTextChanged_Lambda([this](const FText& NewText) { SkillAsset->Description = NewText; })
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
