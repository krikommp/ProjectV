// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DEditModeWidget.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "SlateOptMacros.h"
#define LOCTEXT_NAMESPACE "STilemap3DEditStatusWidget"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STilemap3DEditModeWidget::Construct(const FArguments& InArgs, ETilemap3DEditMode InEditMode)
{
	EditMode = InEditMode;
	OnEditModeChanged = InArgs._OnEditModeChanged;
	ChildSlot
	[
		SNew(SSegmentedControl<int32>)
		.Value(EditMode) // InitialValue
		.OnValueChanged_Lambda( [this] (int32 InValue)
		{
			EditMode = static_cast<ETilemap3DEditMode>(InValue);
			// ReSharper disable once CppExpressionWithoutSideEffects
			OnEditModeChanged.ExecuteIfBound(EditMode);
		} ) 
		.Value_Lambda( [this] { return static_cast<int32>(EditMode); } )  // Bound

		+ SSegmentedControl<int32>::Slot(0)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.SelectComponentTool"))
		  .Text(LOCTEXT("View", "View Mode"))

		+ SSegmentedControl<int32>::Slot(1)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.AddComponentTool"))
		  .Text(LOCTEXT("Append", "Append Cube"))

		+ SSegmentedControl<int32>::Slot(2)
		  .Icon(FAppStyle::Get().GetBrush("LandscapeEditor.DeleteComponentTool"))
		  .Text(LOCTEXT("Remove", "Remove Cube"))
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
