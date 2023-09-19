// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class USkillAsset;
/**
 * SSkillCommonSettingSection
 *
 * 技能通用设置
 */
class PROJECTVEDITOR_API SSkillCommonSettingSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillCommonSettingSection)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakObjectPtr<USkillAsset> InSkillAsset);
protected:
	TWeakObjectPtr<USkillAsset> SkillAsset;
};
