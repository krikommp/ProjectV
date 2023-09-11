// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API SSkillDamageWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillDamageWidget)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<STextBlock> DamageTypeBlock;
	TSharedPtr<STextBlock> DamageAttributeBlock;
	TSharedPtr<STextBlock> DamageCriticalBlock;
	TArray<TSharedPtr<FString>> DamageTypeItems;
	TArray<TSharedPtr<FString>> DamageAttributeItems;
	TArray<TSharedPtr<FString>> DamageCriticalItems;
};
