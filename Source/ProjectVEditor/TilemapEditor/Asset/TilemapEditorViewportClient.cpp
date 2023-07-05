#include "TilemapEditorViewportClient.h"

#include "GridTraceChannel.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"
#include "TilemapEditor/Tilemap3DEditorSettings.h"

FTilemapEditorViewportClient::FTilemapEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene)
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

bool FTilemapEditorViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
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
			
		}
	}
	return FEditorViewportClient::InputKey(EventArgs);
}

void FTilemapEditorViewportClient::DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize,
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
	if (CollisionPlane)
	{
		CollisionPlane->SetVisibility(false);
	}
}

void FTilemapEditorViewportClient::OnTilemapEditStatueChanged(bool Statue)
{
	if (Statue)
	{
		Clear();

		// 初始化
		Blocks.SetNum(TilemapBeingEdited->LevelSizeX * TilemapBeingEdited->LevelSizeY * TilemapBeingEdited->Floors);

		// 绘制编辑区域
		DrawGrid(-1 * FVector(TilemapBeingEdited->GridSize / 2.0f, TilemapBeingEdited->GridSize / 2.0f, 0.0f), TilemapBeingEdited->LevelSizeX, TilemapBeingEdited->LevelSizeY,
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

void FTilemapEditorViewportClient::GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const
{
	ScaleX = TilemapBeingEdited->LevelSizeX * (TilemapBeingEdited->GridSize / 200.0f);
	ScaleY = TilemapBeingEdited->LevelSizeY * (TilemapBeingEdited->GridSize / 200.0f);
	// 注意我们需要计算的是正中间
	const float X = (TilemapBeingEdited->LevelSizeX * TilemapBeingEdited->GridSize) / 2.0f - TilemapBeingEdited->GridSize / 2.0f;
	const float Y = (TilemapBeingEdited->LevelSizeY * TilemapBeingEdited->GridSize) / 2.0f - TilemapBeingEdited->GridSize / 2.0f;

	Location = FVector(X, Y, 0.f);
}
