// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectWindow.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEffectWindow"

void SSkillEffectWindow::Construct(const FArguments& InArgs)
{
	ParentWindowPtr = InArgs._ParentWindow.Get();
	
	SWindow::Construct(SWindow::FArguments()
	.Title(LOCTEXT("SkillEffectDetailEditorLabel", "SkillEffectDetailEditor"))
	.SupportsMinimize(false)
	.SupportsMaximize(false)
	.SizingRule(ESizingRule::FixedSize)
	.AutoCenter(EAutoCenter::PreferredWorkArea)
	.ClientSize(FVector2D(450, 450))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(2)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				]
			]
		]
	]);
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
