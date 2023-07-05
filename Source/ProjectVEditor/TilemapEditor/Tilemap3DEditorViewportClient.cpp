#include "Tilemap3DEditorViewportClient.h"

#include "GridTraceChannel.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"
#include "TilemapEditor/Tilemap3DEditorSettings.h"

FTilemap3DEditorViewportClient::FTilemap3DEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene)
	: FEditorViewportClient(nullptr, &InPreviewScene)
	  , TilemapBeingEdited(InAsset)
{
	SetRealtime(true); //实时绘制

	// 获取线框绘制器
	LineBatcher = PreviewScene->GetLineBatcher();

	const UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();

	// 获取编辑范围可视化组件
	Heightmap = NewObject<UBoxComponent>();
	PreviewScene->AddComponent(Heightmap, FTransform::Identity);
	Heightmap->SetBoxExtent(FVector::One() * TilemapBeingEdited->GridSize);
	Heightmap->SetVisibility(false);

	// 获取碰撞组件
	CollisionPlane = NewObject<UStaticMeshComponent>();
	PreviewScene->AddComponent(CollisionPlane, FTransform::Identity);
	CollisionPlane->SetStaticMesh(Settings->CollisionMesh.LoadSynchronous());
	CollisionPlane->SetMaterial(0, Settings->CollisionPlaneMat.LoadSynchronous());
	CollisionPlane->SetCollisionResponseToChannel(PathTrace, ECR_Block);
	CollisionPlane->SetVisibility(false);

	// 创建地形组件
	TerrainMesh = NewObject<UProceduralMeshComponent>();
	PreviewScene->AddComponent(TerrainMesh, FTransform::Identity);

	SetViewLocation(FVector(0.f, 100.f, 100.f));
	SetLookAtLocation(FVector::Zero(), true);

	HitResultTraceDistance = 10000.0f;

	OnTilemapEditStatueChangedHandle = FTilemap3DEditDelegates::FOnTilemapEditStatueChanged::FDelegate::CreateRaw(
		this, &FTilemap3DEditorViewportClient::OnTilemapEditStatueChanged);
	OnTilemapEditStatueChangedDelegateHandle = FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Add(
		OnTilemapEditStatueChangedHandle);
}

FTilemap3DEditorViewportClient::~FTilemap3DEditorViewportClient()
{
	FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Remove(OnTilemapEditStatueChangedDelegateHandle);
}

void FTilemap3DEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
}

void FTilemap3DEditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(TilemapBeingEdited);
	Collector.AddReferencedObject(Heightmap);
}

bool FTilemap3DEditorViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (EventArgs.Key == EKeys::LeftMouseButton && EventArgs.Event == IE_Pressed)
	{
		FViewportCursorLocation CursorLocation = GetCursorWorldLocationFromMousePos();

		FHitResult HitResult;
		const TArray<AActor*> IgnoreActor;
		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			CursorLocation.GetOrigin(),
			CursorLocation.GetOrigin() + CursorLocation.GetDirection() * HitResultTraceDistance,
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			int32 Index = VectorToIndex(HitResult.Location, 0);
			if (Blocks.IsValidIndex(Index))
			{
				Blocks[Index] = EBlock::Cube;
			}
		}
	}
	return FEditorViewportClient::InputKey(EventArgs);
}

void FTilemap3DEditorViewportClient::DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize,
                                            float ZOffset, const FLinearColor& Color, float Thickness) const
{
	if (LineBatcher)
	{
		for (int32 i = 0; i <= RowCount; ++i)
		{
			LineBatcher->DrawLine(Location + FVector(i * CellSize, 0, ZOffset),
			                      Location + FVector(i * CellSize, ColCount * CellSize, ZOffset), Color, 0, Thickness);
		}

		for (int32 i = 0; i <= ColCount; ++i)
		{
			LineBatcher->DrawLine(Location + FVector(0, i * CellSize, ZOffset),
			                      Location + FVector(RowCount * CellSize, i * CellSize, ZOffset), Color, 0, Thickness);
		}
	}
}

void FTilemap3DEditorViewportClient::Clear() const
{
	if (LineBatcher)
	{
		LineBatcher->Flush();
	}
	if (Heightmap)
	{
		Heightmap->SetVisibility(false);
	}
	if (CollisionPlane)
	{
		CollisionPlane->SetVisibility(false);
	}
}

void FTilemap3DEditorViewportClient::OnTilemapEditStatueChanged(bool Statue)
{
	if (Statue)
	{
		Clear();

		// 初始化
		Blocks.SetNum(TilemapBeingEdited->LevelSizeX * TilemapBeingEdited->LevelSizeY * TilemapBeingEdited->Floors);

		// 绘制编辑区域
		DrawGrid(-1 * FVector(TilemapBeingEdited->GridSize / 2.0f, TilemapBeingEdited->GridSize / 2.0f, 0.0f),
		         TilemapBeingEdited->LevelSizeX, TilemapBeingEdited->LevelSizeY,
		         TilemapBeingEdited->GridSize, 0.f,
		         FLinearColor::White);

		// 绘制编辑范围
		FVector HeightmapLocation;
		float HeightmapScaleX, HeightmapScaleY;
		GetEditRangeScaleAndLocation(HeightmapLocation, HeightmapScaleX,
		                             HeightmapScaleY);
		const float HalfHeight = (TilemapBeingEdited->GetMaxLevelHeight() + TilemapBeingEdited->GetMinLevelHeight()) /
			2.0f;
		const float ScaleHeight = (TilemapBeingEdited->GetMaxLevelHeight() - TilemapBeingEdited->GetMinLevelHeight()) /
			200.0f;
		HeightmapLocation.Z = HalfHeight;
		FTransform HeightmapTransform;
		HeightmapTransform.SetLocation(HeightmapLocation + FVector::Zero());
		HeightmapTransform.SetScale3D(FVector(HeightmapScaleX, HeightmapScaleY, ScaleHeight));
		Heightmap->SetWorldTransform(HeightmapTransform);
		Heightmap->SetVisibility(true);

		// 绘制碰撞器
		FTransform CollisionTransform;
		float CollisionScaleX, CollisionScaleY;
		FVector CollisionLocation;
		GetEditRangeScaleAndLocation(CollisionLocation, CollisionScaleX,
		                             CollisionScaleY);
		CollisionTransform.SetLocation(CollisionLocation + FVector::Zero());
		CollisionTransform.SetScale3D(FVector(CollisionScaleX, CollisionScaleY, 1.0));
		CollisionPlane->SetWorldTransform(CollisionTransform);
		CollisionPlane->SetVisibility(true);
	}
	else
	{
		Clear();
	}
}

void FTilemap3DEditorViewportClient::GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const
{
	ScaleX = TilemapBeingEdited->LevelSizeX * (TilemapBeingEdited->GridSize / 200.0f);
	ScaleY = TilemapBeingEdited->LevelSizeY * (TilemapBeingEdited->GridSize / 200.0f);
	// 注意我们需要计算的是正中间
	const float X = (TilemapBeingEdited->LevelSizeX * TilemapBeingEdited->GridSize) / 2.0f - TilemapBeingEdited->
		GridSize / 2.0f;
	const float Y = (TilemapBeingEdited->LevelSizeY * TilemapBeingEdited->GridSize) / 2.0f - TilemapBeingEdited->
		GridSize / 2.0f;

	Location = FVector(X, Y, 0.f);
}

int32 FTilemap3DEditorViewportClient::VectorToIndex(const FVector& Location, int32 Floor) const
{
	const float PivotX = (TilemapBeingEdited->GridSize * 0.5) + Location.X;
	const float PivotY = (TilemapBeingEdited->GridSize * 0.5) + Location.Y;

	const float ModX = FMath::Floor(FMath::Fmod(PivotX, TilemapBeingEdited->GridSize));
	const float ModY = FMath::Floor(FMath::Fmod(PivotY, TilemapBeingEdited->GridSize));

	const int AddX = ModX ? 1 : 0;
	const int AddY = ModY ? 1 : 0;

	const int X = FMath::Floor(((PivotX + AddX)) / TilemapBeingEdited->GridSize);
	const int Y = FMath::Floor(((PivotY + AddY)) / TilemapBeingEdited->GridSize) *
		TilemapBeingEdited->LevelSizeX;

	const int32 Result = X + Y;

	return Result + TilemapBeingEdited->GridSize * TilemapBeingEdited->GridSize * Floor;
}
