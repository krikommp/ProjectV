// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

enum class ESkillEffectWindowType
{
	Recover,
	State,
	Param,
	Other,
};

/**
 * 
 */
class PROJECTVEDITOR_API SSkillEffectWindow : public SWindow
{
	TSharedRef<SWidget> CreateRecoverContent();
public:
	SLATE_BEGIN_ARGS(SSkillEffectWindow)
		{
		}
	SLATE_ATTRIBUTE( TSharedPtr<SWindow>, ParentWindow )
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
protected:
	TWeakPtr<SWindow> ParentWindowPtr;
	TAttribute<ESkillEffectWindowType> WindowType;
};
