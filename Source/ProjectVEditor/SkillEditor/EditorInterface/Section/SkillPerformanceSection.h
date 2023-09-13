// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class USkillBaseAsset;
/**
 * 
 */
class PROJECTVEDITOR_API SSkillPerformanceSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillPerformanceSection)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
protected:
	TWeakObjectPtr<USkillBaseAsset> SkillAsset;
private:
	TSharedPtr<STextBlock> ComboBoxTitleBlock;
	TSharedPtr<STextBlock> AnimBoxTitleBlock;
	TArray<TSharedPtr<FString>> ComboItems;
	TArray<TSharedPtr<FString>> AnimationItems;
};
