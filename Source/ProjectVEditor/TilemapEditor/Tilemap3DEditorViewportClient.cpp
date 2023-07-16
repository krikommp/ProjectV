#include "Tilemap3DEditorViewportClient.h"

#include "GridTraceChannel.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Tilemap3DEditorManager.h"
#include "Tilemap3DSelected.h"
#include "Tilemap3DTerrainGenerate.h"
#include "Tilemap/TileSet3DAsset.h"
#include "TilemapEditor/Tilemap3DEditorSettings.h"

FTilemap3DEditorViewportClient::FTilemap3DEditorViewportClient(TSharedPtr<STilemap3DPropertiesTabBody> InDetailPtr,
                                                               FPreviewScene& InPreviewScene)
	: FEditorViewportClient(nullptr, &InPreviewScene)
	  , DetailPtr(InDetailPtr)
{
	SetRealtime(true); //实时绘制

	// 获取线框绘制器
	LineBatcher = PreviewScene->GetLineBatcher();

	const UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();

	// 获取编辑范围可视化组件
	Heightmap = NewObject<UBoxComponent>();
	PreviewScene->AddComponent(Heightmap, FTransform::Identity);
	Heightmap->SetBoxExtent(FVector::One() * GetTilemapAsset()->GridSize);
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
	TerrainMat = Settings->DefaultTerrainMesh.LoadSynchronous();

	// 创建预览 actor
	TilemapSelectedPreview = FEditorViewportClient::GetWorld()->SpawnActor<ATilemap3DSelected>(
		ATilemap3DSelected::StaticClass());

	// 获取默认数据
	CurrentTileSet = Settings->DefaultTileSet.LoadSynchronous();

	SetViewLocation(FVector(0.f, 100.f, 100.f));
	SetLookAtLocation(FVector::Zero(), true);

	HitResultTraceDistance = 10000.0f;

	const auto OnTilemapEditStatueChangedDelegate =
		FTilemap3DEditDelegates::FOnTilemapEditStatueChanged::FDelegate::CreateRaw(
			this, &FTilemap3DEditorViewportClient::OnTilemapEditStatueChanged);
	FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Add(
		OnTilemapEditStatueChangedDelegate);

	const auto OnTilemapClearVoxelDelegate = FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FTilemap3DEditorViewportClient::OnTilemapClearVoxel);
	FTilemap3DEditDelegates::OnTilemapClearVoxel.Add(OnTilemapClearVoxelDelegate);

	FTilemap3DTerrainGenerate::Setup(GetTilemapAsset(), TerrainMesh, TerrainMat, this);
	Clear();
}

FTilemap3DEditorViewportClient::~FTilemap3DEditorViewportClient()
{
	FTilemap3DEditDelegates::OnTilemapEditStatueChanged.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapModelChanged.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapClearVoxel.RemoveAll(this);
}

void FTilemap3DEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
}

void FTilemap3DEditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(TilemapSelectedPreview);
	Collector.AddReferencedObject(TerrainMat);
}

bool FTilemap3DEditorViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if ((EventArgs.Key == EKeys::LeftMouseButton || EventArgs.Key == EKeys::RightMouseButton) && EventArgs.Event ==
		IE_Pressed)
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
			if (EventArgs.Key == EKeys::LeftMouseButton)
				FTilemap3DTerrainGenerate::ModifyVoxel(GetTilemapAsset(), TerrainMesh, HitResult.Location,
				                                       GetCurrentTileProperty(),
				                                       GetCurrentFloor(), TerrainMat,
				                                       this);
			else if (EventArgs.Key == EKeys::RightMouseButton)
				FTilemap3DTerrainGenerate::ModifyVoxel(GetTilemapAsset(), TerrainMesh, HitResult.Location,
				                                       FTileSet3DSubObject::EmptyBlock,
				                                       GetCurrentFloor(), TerrainMat,
				                                       this);
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
		CollisionPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

int32 FTilemap3DEditorViewportClient::GetBlockTextureIndex(const FName& ID, int32 Flag) const
{
	if (const auto& TileSet = GetTileSet())
	{
		for (FTileSet3DSubObject Tile : TileSet->TileSets)
		{
			if (Tile.ID == ID)
			{
				UTexture2D* Tex;
				if (Tile.BlockTextures.Num() <= Flag)
				{
					Tex = Tile.BlockTextures[0];
				}
				else
				{
					Tex = Tile.BlockTextures[Flag];
				}

				return DetailPtr->GetTextureIndex(Tex);
			}
		}
	}
	return -1;
}

void FTilemap3DEditorViewportClient::OnTilemapEditStatueChanged(bool Statue)
{
	if (Statue)
	{
		Clear();

		// 初始化
		if (GetTilemapAsset()->LevelSizeX * GetTilemapAsset()->LevelSizeY * GetTilemapAsset()->Floors !=
			GetTilemapAsset()->Blocks.Num())
		{
			GetTilemapAsset()->Blocks.SetNum(
				GetTilemapAsset()->LevelSizeX * GetTilemapAsset()->LevelSizeY * GetTilemapAsset()->Floors);
		}

		// 绘制编辑区域
		DrawGrid(-1 * FVector(GetTilemapAsset()->GridSize / 2.0f, GetTilemapAsset()->GridSize / 2.0f, 0.0f),
		         GetTilemapAsset()->LevelSizeX, GetTilemapAsset()->LevelSizeY,
		         GetTilemapAsset()->GridSize, GetCurrentFloor() * GetTilemapAsset()->HeightSize,
		         FLinearColor::White);

		// 绘制编辑范围
		FVector HeightmapLocation;
		float HeightmapScaleX, HeightmapScaleY;
		GetEditRangeScaleAndLocation(HeightmapLocation, HeightmapScaleX,
		                             HeightmapScaleY);
		const float HalfHeight = (GetTilemapAsset()->GetMaxLevelHeight() + GetTilemapAsset()->GetMinLevelHeight()) /
			2.0f;
		const float ScaleHeight = (GetTilemapAsset()->GetMaxLevelHeight() - GetTilemapAsset()->GetMinLevelHeight()) /
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
		CollisionLocation.Z += GetCurrentFloor() * GetTilemapAsset()->HeightSize;
		CollisionTransform.SetLocation(CollisionLocation + FVector::Zero());
		CollisionTransform.SetScale3D(FVector(CollisionScaleX, CollisionScaleY, 1.0));
		CollisionPlane->SetWorldTransform(CollisionTransform);
		CollisionPlane->SetVisibility(true);
		CollisionPlane->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		Clear();
	}
}

void FTilemap3DEditorViewportClient::OnTilemapClearVoxel()
{
	FTilemap3DTerrainGenerate::ClearVoxel(GetTilemapAsset(), TerrainMesh, TerrainMat);
}

void FTilemap3DEditorViewportClient::GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const
{
	ScaleX = GetTilemapAsset()->LevelSizeX * (GetTilemapAsset()->GridSize / 200.0f);
	ScaleY = GetTilemapAsset()->LevelSizeY * (GetTilemapAsset()->GridSize / 200.0f);
	// 注意我们需要计算的是正中间
	const float X = (GetTilemapAsset()->LevelSizeX * GetTilemapAsset()->GridSize) / 2.0f - GetTilemapAsset()->
		GridSize / 2.0f;
	const float Y = (GetTilemapAsset()->LevelSizeY * GetTilemapAsset()->GridSize) / 2.0f - GetTilemapAsset()->
		GridSize / 2.0f;

	Location = FVector(X, Y, 0.f);
}
