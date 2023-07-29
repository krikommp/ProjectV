// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapEditor/Tilemap3DEditorManager.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSegmentedControl.h"

/**
 * 
 */
class PROJECTVEDITOR_API STilemap3DEditModeWidget : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnEditModeChanged, const ETilemap3DEditMode)
	SLATE_BEGIN_ARGS(STilemap3DEditModeWidget)
		: _EditMode(EEM_View), _BaseEditMode(EEM_View)
		{
		}

		SLATE_ATTRIBUTE(ETilemap3DEditMode, EditMode)
		SLATE_ATTRIBUTE(ETilemap3DEditMode, BaseEditMode)
		SLATE_EVENT(FOnEditModeChanged, OnEditModeChanged)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	TAttribute<ETilemap3DEditMode> EditMode;
	TAttribute<ETilemap3DEditMode> BaseEditMode;
	FOnEditModeChanged OnEditModeChanged;

	TSharedPtr<SSegmentedControl<int32>> CubePanelControl;
	TSharedPtr<SSegmentedControl<int32>> MeshPanelControl;
	TSharedPtr<SSegmentedControl<int32>> ChessPanelControl;
};
