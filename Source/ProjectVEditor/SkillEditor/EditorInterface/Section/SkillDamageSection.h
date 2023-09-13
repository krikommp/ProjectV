// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class USkillBaseAsset;
/**
 * 
 */
class PROJECTVEDITOR_API SSkillDamageSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillDamageSection)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
protected:
	TWeakObjectPtr<USkillBaseAsset> SkillAsset;
private:
	TSharedPtr<STextBlock> DamageTypeBlock;
	TSharedPtr<STextBlock> DamageAttributeBlock;
	TSharedPtr<STextBlock> DamageCriticalBlock;
	TArray<TSharedPtr<FString>> DamageTypeItems;
	TArray<TSharedPtr<FString>> DamageAttributeItems;
	TArray<TSharedPtr<FString>> DamageCriticalItems;
};
