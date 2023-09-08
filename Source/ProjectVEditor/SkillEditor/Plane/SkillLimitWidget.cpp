// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillLimitWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillInfoWidget"

void SSkillLimitWidget::Construct(const FArguments& InArgs)
{
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 1").ToString())));
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 2").ToString())));

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
				.Text(LOCTEXT("SkillLimitTitleLabel", "Skill Limit"))
			]
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Top)
			  .Padding(10.0, 10.0f)
			  .AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Fill)
				  .FillWidth(1.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillWeapoenType_1_Title", "Weapoen type 1:"))
					]
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(SComboBox<TSharedPtr<FString> >)
						.OptionsSource(&WeaponTypeItems)
						.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
						                                     {
							                                     return SNew(STextBlock).Text(FText::FromString(*Item));
						                                     })
						.OnSelectionChanged_Lambda(
							                                     [this](TSharedPtr<FString> InSelection,
							                                            ESelectInfo::Type InSelectInfo)
							                                     {
								                                     if (InSelection.IsValid() && WeaponTypeTitleBlock_1
									                                     .
									                                     IsValid())
								                                     {
									                                     WeaponTypeTitleBlock_1->SetText(
										                                     FText::FromString(*InSelection));
								                                     }
							                                     })
						[
							SAssignNew(WeaponTypeTitleBlock_1, STextBlock).Text(
								LOCTEXT("SkillWeaponType_1_Label", "None"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				  .HAlign(HAlign_Fill)
				  .Padding(10.0f, 0.0f)
				  .FillWidth(1.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SkillWeapoenType_2_Title", "Weapoen type 2:"))
					]
					+ SVerticalBox::Slot()
					  .VAlign(VAlign_Top)
					  .AutoHeight()
					[
						SNew(SComboBox<TSharedPtr<FString> >)
						.OptionsSource(&WeaponTypeItems)
						.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
						                                     {
							                                     return SNew(STextBlock).Text(FText::FromString(*Item));
						                                     })
						.OnSelectionChanged_Lambda(
							                                     [this](TSharedPtr<FString> InSelection,
							                                            ESelectInfo::Type InSelectInfo)
							                                     {
								                                     if (InSelection.IsValid() && WeaponTypeTitleBlock_2
									                                     .
									                                     IsValid())
								                                     {
									                                     WeaponTypeTitleBlock_2->SetText(
										                                     FText::FromString(*InSelection));
								                                     }
							                                     })
						[
							SAssignNew(WeaponTypeTitleBlock_2, STextBlock).Text(
								LOCTEXT("SkillWeaponType_1_Label", "None"))
						]
					]

				]
			]
		]
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
