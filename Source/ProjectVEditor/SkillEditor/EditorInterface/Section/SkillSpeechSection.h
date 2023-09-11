// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PROJECTVEDITOR_API SSkillSpeechSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillSpeechSection)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
private:
	TArray<TSharedPtr<FString>> VoiceItems;
	TSharedPtr<STextBlock> VoiceBoxTitleBlock;
};
