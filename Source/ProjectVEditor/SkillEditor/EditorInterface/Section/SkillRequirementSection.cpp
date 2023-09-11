// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillRequirementSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Widgets/Input/SSpinBox.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillRequirementSection"
void SSkillRequirementSection::Construct(const FArguments& InArgs)
{
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "None").ToString())));
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 1").ToString())));
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 2").ToString())));

	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);
	TSharedRef<SHorizontalBox> WeaponTypeHBox = SNew(SHorizontalBox);

	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillLimitTitleLabel", "Skill Limit"));
	NextHSlot(WeaponTypeHBox, LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 1"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&WeaponTypeItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		                                     {
			                                     return SNew(STextBlock).Text(FText::FromString(*Item));
		                                     })
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		                                     {
			                                     if (InSelection.IsValid() && WeaponTypeTitleBlock_1.IsValid())
			                                     {
				                                     WeaponTypeTitleBlock_1->SetText(FText::FromString(*InSelection));
			                                     }
		                                     })
		[
			SAssignNew(WeaponTypeTitleBlock_1, STextBlock).Text(
				LOCTEXT("SkillWeaponType_1_Label", "None"))
		]
	];
	NextHSlot(WeaponTypeHBox, LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 2"))
	[
		SNew(SComboBox<TSharedPtr<FString> >)
		.OptionsSource(&WeaponTypeItems)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
		                                     {
			                                     return SNew(STextBlock).Text(FText::FromString(*Item));
		                                     })
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo)
		                                     {
			                                     if (InSelection.IsValid() && WeaponTypeTitleBlock_2.IsValid())
			                                     {
				                                     WeaponTypeTitleBlock_2->SetText(FText::FromString(*InSelection));
			                                     }
		                                     })
		[
			SAssignNew(WeaponTypeTitleBlock_2, STextBlock).Text(
				LOCTEXT("SkillWeaponType_2_Label", "None"))
		]
	];

	NextVSlot(WidgetVerticalBox)
	[
		WeaponTypeHBox
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
