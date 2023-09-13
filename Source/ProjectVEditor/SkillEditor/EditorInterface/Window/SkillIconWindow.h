// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class USkillBaseAsset;
/**
 * 
 */
class PROJECTVEDITOR_API SSkillIconWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SSkillIconWindow)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakObjectPtr<USkillBaseAsset> InSkillAsset);

protected:
	FReply OnImageClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, const int32 TexIndex);

	TWeakObjectPtr<USkillBaseAsset> SkillAsset;
	TArray<TObjectPtr<UTexture2D>> IconTexArray;
};
