#include "Tilemap3DEditorViewport.h"
#include "Tilemap3DEditorViewportClient.h"

void STilemap3DEditorViewport::Construct(const FArguments& InArgs)
{
	DetailPtr = InArgs._TilemapDetailPtr.Get();
	PreviewScene = MakeShareable(new FPreviewScene());
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

STilemap3DEditorViewport::STilemap3DEditorViewport()
{
}

STilemap3DEditorViewport::~STilemap3DEditorViewport()
{
}

TSharedRef<SEditorViewport> STilemap3DEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> STilemap3DEditorViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void STilemap3DEditorViewport::OnFloatingButtonClicked()
{
}

TSharedPtr<SWidget> STilemap3DEditorViewport::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this));
}

TSharedRef<FEditorViewportClient> STilemap3DEditorViewport::MakeEditorViewportClient()
{
	if (!TilemapAssetPreviewClient.IsValid())
	{
		TilemapAssetPreviewClient = MakeShareable(
			new FTilemap3DEditorViewportClient(DetailPtr, *PreviewScene));
	}

	return TilemapAssetPreviewClient.ToSharedRef();
}
