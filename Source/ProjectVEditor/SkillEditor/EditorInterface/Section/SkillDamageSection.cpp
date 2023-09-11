// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDamageSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SSpinBox.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillDamageSection"
void SSkillDamageSection::Construct(const FArguments& InArgs)
{
	DamageTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageTypeItemLabel", "None").ToString())));
	DamageTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageTypeItemLabel", "HP Damage").ToString())));
	DamageTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageTypeItemLabel", "TP Damage").ToString())));

	DamageAttributeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageAttributeItemLabel", "None").ToString())));
	DamageAttributeItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageAttributeItemLabel", "Normal Attack").ToString())));

	DamageCriticalItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageCriticalItemLabel", "Yes").ToString())));
	DamageCriticalItems.Add(MakeShareable(new FString(LOCTEXT("SkillDamageCriticalItemLabel", "No").ToString())));
	
	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);
	TSharedRef<SHorizontalBox> SkillDamageHBox_1 = SNew(SHorizontalBox);
	
	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillDamgeTitleLabel", "Damage"));
	NextHSlot(SkillDamageHBox_1, LOCTEXT("SkillDamageTypeLabel", "Type"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&DamageTypeItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		{
			return SNew(STextBlock).Text(FText::FromString(*Item));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		{
			if (InSelection.IsValid() && DamageTypeBlock.IsValid())
			{
				DamageTypeBlock->SetText(FText::FromString(*InSelection));
			}
		})
		[
			SAssignNew(DamageTypeBlock, STextBlock).Text(LOCTEXT("SkillDamageTypeItemLabel", "None"))
		]
	];
	NextHSlot(SkillDamageHBox_1, LOCTEXT("SkillDamageAttributeLabel", "Attribute"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&DamageAttributeItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		{
			return SNew(STextBlock).Text(FText::FromString(*Item));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		{
			if (InSelection.IsValid() && DamageAttributeBlock.IsValid())
			{
				DamageAttributeBlock->SetText(FText::FromString(*InSelection));
			}
		})
		[
			SAssignNew(DamageAttributeBlock, STextBlock).Text(LOCTEXT("SkillDamageAttributeItemLabel", "None"))
		]
	];
	NextVSlot(WidgetVerticalBox)
	[
		SkillDamageHBox_1
	];
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillDamageFormula", "Damage Formula"))
	[
		SNew(SEditableTextBox)
		.HintText(LOCTEXT("SkillDamageFormula", "Damage Formula"))
	];
	NextVSlot(WidgetVerticalBox, LOCTEXT("SkillDamageCriticalLabel", "Critical"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&DamageCriticalItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		{
			return SNew(STextBlock).Text(FText::FromString(*Item));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		{
			if (InSelection.IsValid() && DamageCriticalBlock.IsValid())
			{
				DamageCriticalBlock->SetText(FText::FromString(*InSelection));
			}
		})
		[
			SAssignNew(DamageCriticalBlock, STextBlock).Text(LOCTEXT("SkillDamageCriticalItemLabel", "No"))
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
