// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API SSkillRequirementSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillRequirementSection)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
private:
	TSharedPtr<STextBlock> WeaponTypeTitleBlock_1;
	TSharedPtr<STextBlock> WeaponTypeTitleBlock_2;
	TArray<TSharedPtr<FString>> WeaponTypeItems;
};
