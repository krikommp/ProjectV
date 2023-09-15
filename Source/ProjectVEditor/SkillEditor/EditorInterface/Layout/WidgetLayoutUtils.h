#pragma once

DECLARE_DELEGATE_OneParam(FOnSlotCheckStateChanged, bool);

PROJECTVEDITOR_API void TitleSlot(TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel);
PROJECTVEDITOR_API SVerticalBox::FScopedWidgetSlotArguments NextVSlot(TSharedPtr<SVerticalBox> VerticalBox, const FText& InLabel = FText::GetEmpty());
PROJECTVEDITOR_API SVerticalBox::FScopedWidgetSlotArguments NextHSlot(TSharedPtr<SHorizontalBox> HorizontalBox, const FText& InLabel = FText::GetEmpty(), EHorizontalAlignment HAlign = HAlign_Fill);
PROJECTVEDITOR_API SHorizontalBox::FScopedWidgetSlotArguments NextVCheckSlot(TSharedPtr<SVerticalBox> VerticalBox, TArray<TSharedPtr<SHorizontalBox>>& ContentBoxArray, const FText& InLabel = FText::GetEmpty());