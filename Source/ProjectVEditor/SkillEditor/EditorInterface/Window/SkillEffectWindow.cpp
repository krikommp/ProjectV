// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectWindow.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SkillEffectWindow::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

SkillEffectWindow::SkillEffectWindow()
{
}

SkillEffectWindow::~SkillEffectWindow()
{
}

TSharedRef<SEditorViewport> SkillEffectWindow::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SkillEffectWindow::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SkillEffectWindow::OnFloatingButtonClicked()
{
}

TSharedPtr<SWidget> SkillEffectWindow::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this));
}

TSharedRef<FEditorViewportClient> SkillEffectWindow::MakeEditorViewportClient()
{
	PreviewScene = MakeShareable(new FPreviewScene());

	TSharedPtr<FEditorViewportClient> EditorViewportClient = MakeShareable(
		new FEditorViewportClient(nullptr, PreviewScene.Get()));

	return EditorViewportClient.ToSharedRef();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
