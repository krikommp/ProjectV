#include "WidgetLayoutUtils.h"

void TitleSlot(TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel)
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
}

SVerticalBox::FScopedWidgetSlotArguments NextVSlot(TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel)
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
}

SVerticalBox::FScopedWidgetSlotArguments NextHSlot(TSharedPtr<SHorizontalBox> HorizontalBox, const FText& InLabel, EHorizontalAlignment HAlign)
{
	TSharedRef<SVerticalBox> VBox = SNew(SVerticalBox);

	HorizontalBox->AddSlot()
				 .HAlign(HAlign)
				 .FillWidth(1.0f)
				 .Padding(HorizontalBox->NumSlots() == 0 ? 0.0f : 10.0f, 0.0f)
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
}
