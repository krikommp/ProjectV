// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API STilemap3DEditStatusWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STilemap3DEditStatusWidget)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, bool InDefaultEditStatus);

private:
	TSharedPtr<STextBlock> StatusText;
	bool bEdit = false;
};
