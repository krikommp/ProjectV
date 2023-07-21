﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

enum ETilemap3DEditMode : uint8
{
	EEM_View = 0,
	EEM_Append = 1,
	EEM_Remove = 2,
};

/**
 * 
 */
class PROJECTVEDITOR_API STilemap3DEditModeWidget : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnEditModeChanged, const ETilemap3DEditMode)
	SLATE_BEGIN_ARGS(STilemap3DEditModeWidget)
		{
		}

	SLATE_EVENT(FOnEditModeChanged, OnEditModeChanged)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, ETilemap3DEditMode InEditMode);

private:
	ETilemap3DEditMode EditMode = ETilemap3DEditMode::EEM_Append;
	FOnEditModeChanged OnEditModeChanged;
};