#include "EditorWindowViewport.h"

void SEditorWindowViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

SEditorWindowViewport::SEditorWindowViewport()
{
}

SEditorWindowViewport::~SEditorWindowViewport()
{
}

TSharedRef<SEditorViewport> SEditorWindowViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SEditorWindowViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SEditorWindowViewport::OnFloatingButtonClicked()
{
}

TSharedPtr<SWidget> SEditorWindowViewport::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this));
}

TSharedRef<FEditorViewportClient> SEditorWindowViewport::MakeEditorViewportClient()
{
	PreviewScene = MakeShareable(new FPreviewScene());

	TSharedPtr<FEditorViewportClient> EditorViewportClient = MakeShareable(
		new FEditorViewportClient(nullptr, PreviewScene.Get()));

	return EditorViewportClient.ToSharedRef();
}
