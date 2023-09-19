// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class USkillAsset;
/**
 * SSkillEditorLayout
 *
 * 技能编辑器基本布局
 */
class PROJECTVEDITOR_API SSkillEditorLayout : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillEditorLayout)
		{
		}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakObjectPtr<USkillAsset> InSkillAsset);

protected:
	TWeakObjectPtr<USkillAsset> SkillAsset;
};
