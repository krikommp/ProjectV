// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillLimitWidget.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillInfoWidget"

void SSkillLimitWidget::Construct(const FArguments& InArgs)
{
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "None").ToString())));
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 1").ToString())));
	WeaponTypeItems.Add(MakeShareable(new FString(LOCTEXT("SkillWeaponTypeLabel", "Weapon Type 2").ToString())));

	auto TitleSlot = [](TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel)
	{
		VerticalBox->AddSlot()
		           .VAlign(VAlign_Top)
		           .Padding(10.0f, 10.0f)
		           .AutoHeight()
		[
			SNew(STextBlock)
    			.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
    			.Text(InLabel)
		];
	};

	auto NextVSlot = [](TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel = FText::GetEmpty())
	{
		TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);

		VerticalBox->AddSlot()
		           .VAlign(VAlign_Top)
		           .Padding(10.0f, 10.0f)
		           .AutoHeight()
		[
			VBox
		];

		if (!InLabel.IsEmpty())
		{
			VBox->AddSlot()
			    .VAlign(VAlign_Top)
			    .AutoHeight()
			[
				SNew(STextBlock)
				.Text(InLabel)
			];
		}

		SVerticalBox::FScopedWidgetSlotArguments NewSlot = VBox->AddSlot();
		NewSlot.VAlign(VAlign_Top)
		       .AutoHeight();
		return MoveTemp(NewSlot);
	};

	auto NextHSlot = [](TSharedPtr<SHorizontalBox> HorizontalBox, const FText& InLabel)
	{
		TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);

		HorizontalBox->AddSlot()
		             .HAlign(HAlign_Fill)
		             .FillWidth(1.0f)
		             .Padding(HorizontalBox->NumSlots() == 0 ? 0.0f : 10.0f, 0.0f)
		[
			VBox
		];

		VBox->AddSlot()
		    .VAlign(VAlign_Top)
		    .AutoHeight()
		[
			SNew(STextBlock)
			.Text(InLabel)
		];

		SVerticalBox::FScopedWidgetSlotArguments NewSlot = VBox->AddSlot();
		NewSlot.VAlign(VAlign_Top)
		       .AutoHeight();
		return MoveTemp(NewSlot);
	};

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
