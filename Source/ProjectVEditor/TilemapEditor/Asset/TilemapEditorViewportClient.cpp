#include "TilemapEditorViewportClient.h"

#include "Components/LineBatchComponent.h"
#include "Tilemap/TilemapAsset.h"

FTilemapEditorViewportClient::FTilemapEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene)
	: FEditorViewportClient(nullptr, &InPreviewScene)
	  , TilemapBeingEdited(InAsset)
{
	SetRealtime(true); //实时绘制
	LineBatcher = PreviewScene->GetLineBatcher();
	SetViewLocation(FVector(0.f, 0.f, 100.f));

	OnTilemapEditStatueChangedHandle = FTilemapEditDelegates::FOnTilemapEditStatueChanged::FDelegate::CreateRaw(this, &FTilemapEditorViewportClient::OnTilemapEditStatueChanged);
	OnTilemapEditStatueChangedDelegateHandle = FTilemapEditDelegates::OnTilemapEditStatueChanged.Add(OnTilemapEditStatueChangedHandle);
}

FTilemapEditorViewportClient::~FTilemapEditorViewportClient()
{
	FTilemapEditDelegates::OnTilemapEditStatueChanged.Remove(OnTilemapEditStatueChangedDelegateHandle);
}

void FTilemapEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
}

void FTilemapEditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(TilemapBeingEdited);
}

void FTilemapEditorViewportClient::DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize,
                                            float ZOffset, const FLinearColor& Color, float Thickness) const
{
	if (LineBatcher)
	{
		for (int32 i = 0; i <= RowCount; ++i)
		{
			LineBatcher->DrawLine(Location + FVector(0, i * CellSize, ZOffset),
								  Location + FVector(ColCount * CellSize, i * CellSize, ZOffset), Color, 0, Thickness);
		}

		for (int32 i = 0; i <= ColCount; ++i)
		{
			LineBatcher->DrawLine(Location + FVector(i * CellSize, 0, ZOffset),
								  Location + FVector(i * CellSize, ColCount * CellSize, ZOffset), Color, 0, Thickness);
		}
	}
}

void FTilemapEditorViewportClient::Clear() const
{
	if (LineBatcher)
	{
		LineBatcher->Flush();
	}
}

void FTilemapEditorViewportClient::OnTilemapEditStatueChanged(bool Statue)
{
	if (Statue)
	{
		Clear();

		// 绘制编辑区域
		DrawGrid(FVector::Zero(), TilemapBeingEdited->LevelWidth, TilemapBeingEdited->LevelDepth, 100, 100, FLinearColor::White);
	}else
	{
		Clear();
	}
}
