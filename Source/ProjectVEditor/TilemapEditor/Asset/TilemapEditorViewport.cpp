#include "TilemapEditorViewport.h"
#include "TilemapEditorViewportClient.h"

void STilemapEditorViewport::Construct(const FArguments& InArgs)
{
	TilemapBeingEdited = InArgs._TilemapBeingEdited.Get();
	PreviewScene = MakeShareable(new FPreviewScene());
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

STilemapEditorViewport::STilemapEditorViewport()
{
}

STilemapEditorViewport::~STilemapEditorViewport()
{
}

TSharedRef<SEditorViewport> STilemapEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> STilemapEditorViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void STilemapEditorViewport::OnFloatingButtonClicked()
{
}

TSharedPtr<SWidget> STilemapEditorViewport::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this));
}

TSharedRef<FEditorViewportClient> STilemapEditorViewport::MakeEditorViewportClient()
{
	if (!TilemapAssetPreviewClient.IsValid())
	{
		TilemapAssetPreviewClient = MakeShareable(
			new FTilemapEditorViewportClient(TilemapBeingEdited, *PreviewScene));
	}

	return TilemapAssetPreviewClient.ToSharedRef();
}
