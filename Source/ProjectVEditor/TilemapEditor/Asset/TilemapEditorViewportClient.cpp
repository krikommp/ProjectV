#include "TilemapEditorViewportClient.h"

#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Tilemap/TilemapAsset.h"

FTilemapEditorViewportClient::FTilemapEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene)
	: FEditorViewportClient(nullptr, &InPreviewScene)
	  , TilemapBeingEdited(InAsset)
{
	SetRealtime(true); //实时绘制

	// 获取线框绘制器
	LineBatcher = PreviewScene->GetLineBatcher();

	// 获取编辑范围可视化组件
	Heightmap = NewObject<UBoxComponent>();
	PreviewScene->AddComponent(Heightmap, FTransform::Identity);
	Heightmap->SetBoxExtent(FVector::One() * TilemapBeingEdited->GridSize);
	Heightmap->SetVisibility(false);

	SetViewLocation(FVector(0.f, 100.f, 100.f));
	SetLookAtLocation(FVector::Zero(), true);

	OnTilemapEditStatueChangedHandle = FTilemapEditDelegates::FOnTilemapEditStatueChanged::FDelegate::CreateRaw(
		this, &FTilemapEditorViewportClient::OnTilemapEditStatueChanged);
	OnTilemapEditStatueChangedDelegateHandle = FTilemapEditDelegates::OnTilemapEditStatueChanged.Add(
		OnTilemapEditStatueChangedHandle);
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
	Collector.AddReferencedObject(Heightmap);
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
	if (Heightmap)
	{
		Heightmap->SetVisibility(false);
	}
}

void FTilemapEditorViewportClient::OnTilemapEditStatueChanged(bool Statue)
{
	if (Statue)
	{
		Clear();

		// 绘制编辑区域
		DrawGrid(FVector::Zero(), TilemapBeingEdited->LevelSizeX, TilemapBeingEdited->LevelSizeY,
		         TilemapBeingEdited->GridSize, 0.f,
		         FLinearColor::White);

		// 绘制编辑范围
		FVector HeightmapLocation;
		float HeightmapScaleX, HeightmapScaleY;
		GetEditRangeScaleAndLocation(HeightmapLocation, HeightmapScaleX,
		                             HeightmapScaleY);
		const float HalfHeight = (TilemapBeingEdited->GetMaxLevelHeight() + TilemapBeingEdited->GetMinLevelHeight()) / 2.0f;
		const float ScaleHeight = (TilemapBeingEdited->GetMaxLevelHeight() - TilemapBeingEdited->GetMinLevelHeight()) / 200.0f;
		HeightmapLocation.Z = HalfHeight;
		FTransform HeightmapTransform;
		HeightmapTransform.SetLocation(HeightmapLocation + FVector::Zero());
		HeightmapTransform.SetScale3D(FVector(HeightmapScaleX, HeightmapScaleY, ScaleHeight));
		Heightmap->SetWorldTransform(HeightmapTransform);
		Heightmap->SetVisibility(true);
	}
	else
	{
		Clear();
	}
}

void FTilemapEditorViewportClient::GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const
{
	ScaleX = TilemapBeingEdited->LevelSizeX * (TilemapBeingEdited->GridSize / 200.0f);
	ScaleY = TilemapBeingEdited->LevelSizeY * (TilemapBeingEdited->GridSize / 200.0f);
	// 注意我们需要计算的是正中间
	const float X = (TilemapBeingEdited->LevelSizeX * TilemapBeingEdited->GridSize / 2.0f) - (TilemapBeingEdited->
		GridSize /
		2.0f);
	const float Y = (TilemapBeingEdited->LevelSizeY * TilemapBeingEdited->GridSize) / 2.0f - (TilemapBeingEdited->
		GridSize /
		2.0f);

	Location = FVector(X, Y, 0.1);
}
