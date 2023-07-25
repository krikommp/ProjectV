// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API STilemap3DFloorStatusWidget : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnTilemapFloorChanged, const int32);
	SLATE_BEGIN_ARGS(STilemap3DFloorStatusWidget)
		{
		}
	SLATE_ATTRIBUTE(int32, Floor)
	SLATE_EVENT(FOnTilemapFloorChanged, OnFloorIncrease)
	SLATE_EVENT(FOnTilemapFloorChanged, OnFloorDecrease)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	TAttribute<int32> Floor;
	FOnTilemapFloorChanged OnFloorIncrease;
	FOnTilemapFloorChanged OnFloorDecrease;
};
