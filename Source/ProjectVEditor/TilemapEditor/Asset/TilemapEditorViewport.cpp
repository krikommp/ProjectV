#include "TilemapEditorViewport.h"

#include "Components/LineBatchComponent.h"

void STilemapEditorViewport::Construct(const FArguments& InArgs)
{
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

void STilemapEditorViewport::DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize,
	float ZOffset, const FLinearColor& Color, float Thickness) const
{
	if (LineBatcher)
	{
		for (int32 i = 0; i <= RowCount; ++i)
		{
			LineBatcher->DrawLine(Location + FVector(0, i * CellSize, ZOffset), Location + FVector(ColCount * CellSize, i * CellSize, ZOffset), Color, 0, Thickness);
		}

		for (int32 i = 0; i <= ColCount; ++i)
		{
			LineBatcher->DrawLine(Location + FVector(i * CellSize, 0, ZOffset), Location + FVector(i * CellSize, ColCount * CellSize, ZOffset), Color, 0, Thickness);
		}
	}
}

TSharedRef<FEditorViewportClient> STilemapEditorViewport::MakeEditorViewportClient()
{
	PreviewScene = MakeShareable(new FPreviewScene());

	LineBatcher = PreviewScene->GetLineBatcher();

	TSharedPtr<FEditorViewportClient> EditorViewportClient = MakeShareable(
		new FEditorViewportClient(nullptr, PreviewScene.Get()));

	return EditorViewportClient.ToSharedRef();
}
