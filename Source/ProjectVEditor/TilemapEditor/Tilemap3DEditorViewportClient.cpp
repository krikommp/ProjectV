#include "Tilemap3DEditorViewportClient.h"

#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"
#include "ProjectVEditor.h"
#include "Tilemap3DEditorManager.h"
#include "Tilemap3DSelected.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Generator/Tilemap3DPathfindingGenerator.h"
#include "Generator/Tilemap3DTerrainGenerator.h"
#include "Generator/Tilemap3DTileMeshGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Mode/Tilemap3DAddCubeMode.h"
#include "Mode/Tilemap3DAddMeshMode.h"
#include "Mode/Tilemap3DRemoveCubeMode.h"
#include "Mode/Tilemap3DRemoveMeshMode.h"
#include "Mode/Tilemap3DSelectMeshMode.h"
#include "StateMachine/Tilemap3DEditeModeStateMachine.h"
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
	CollisionPlane->SetCollisionResponseToChannel(TilemapEditTrace, ECR_Block);
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
	CachedTilemapSize.SetNumUninitialized(3);
	CachedTilemapSize[0] = GetTilemapAsset()->LevelSizeX;
	CachedTilemapSize[1] = GetTilemapAsset()->LevelSizeY;
	CachedTilemapSize[2] = GetTilemapAsset()->Floors;

	EditMode = TAttribute<ETilemap3DEditMode>::CreateLambda([this](){ return DetailPtr->GetEditMode(); });
	const auto OnTilemapEditModeChangedDelegate = FTilemap3DEditDelegates::FOnTilemapEditModeChanged::FDelegate::CreateRaw(
	this, &FTilemap3DEditorViewportClient::OnTilemapEditModeChanged);
	FTilemap3DEditDelegates::OnTilemapEditModeChanged.Add(OnTilemapEditModeChangedDelegate);
	StateMachine = MakeUnique<FTilemap3DEditeModeStateMachine>();
	StateMachine->RegisterEditMode<FTilemap3DAddCubeMode>(ETilemap3DEditMode::EEM_Cube_Append);
	StateMachine->RegisterEditMode<FTilemap3DRemoveCubeMode>(ETilemap3DEditMode::EEM_Cube_Remove);
	StateMachine->RegisterEditMode<FTilemap3DAddMeshMode>(ETilemap3DEditMode::EEM_Mesh_Append);
	StateMachine->RegisterEditMode<FTilemap3DSelectMeshMode>(ETilemap3DEditMode::EEM_Mesh_Select);
	StateMachine->RegisterEditMode<FTilemap3DRemoveMeshMode>(ETilemap3DEditMode::EEM_Mesh_Remove);

	const auto OnTilemapFillFloorDelegate = FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTilemap3DEditorViewportClient::OnTilemapFillFloor);
	FTilemap3DEditDelegates::OnTilemapFillFloor.Add(OnTilemapFillFloorDelegate);
	
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

	const auto OnTilemapGeneratePathFindingDelegate = FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FTilemap3DEditorViewportClient::OnTilemapGeneratePathFinding);
	FTilemap3DEditDelegates::OnTilemapGeneratePathFinding.Add(OnTilemapGeneratePathFindingDelegate);

	FTilemap3DTerrainGenerator::Setup(GetTilemapAsset(), TerrainMesh, TerrainMat, this);
	FTilemap3DTileMeshGenerator::Setup(GetTilemapAsset(), MeshSet, PreviewScene, GetTileSet());

	Clear();
}

FTilemap3DEditorViewportClient::~FTilemap3DEditorViewportClient()
{
	FTilemap3DEditDelegates::OnTilemapEditStatueChanged.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapModelChanged.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapClearVoxel.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapGeneratePathFinding.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapEditModeChanged.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapFillFloor.RemoveAll(this);
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
	// for (const auto& Mode : EditModes)
	// {
	// 	Mode->InputKey(this, EventArgs);
	// }
	StateMachine->InputKey(this, EventArgs);
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

void FTilemap3DEditorViewportClient::AddMesh(const FName& ID, UStaticMesh* Mesh, const FTransform& Transform, int32 Index)
{
	UInstancedStaticMeshComponent* MeshComponent;
	if (MeshSet.Contains(ID))
	{
		MeshComponent = MeshSet[ID];
	}else
	{
		MeshComponent = NewObject<UInstancedStaticMeshComponent>();
		MeshComponent->SetStaticMesh(Mesh);
		PreviewScene->AddComponent(MeshComponent, FTransform::Identity);
		MeshSet.Add(ID, MeshComponent);
	}
}

int32 FTilemap3DEditorViewportClient::GetBlockTextureIndex(const FName& ID, int32 Flag) const
{
	if (const auto& TileSet = GetTileSet())
	{
		for (FTileSet3DCube Tile : TileSet->TileCubeSets)
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
			TArray<FBlock> NewBlocks;
			NewBlocks.SetNum(GetTilemapAsset()->LevelSizeX * GetTilemapAsset()->LevelSizeY * GetTilemapAsset()->Floors);
			for (int32 x = 0; x < GetTilemapAsset()->LevelSizeX; ++x)
			{
				for (int32 y = 0; y < GetTilemapAsset()->LevelSizeY; ++y)
				{
					for (int32 z = 0; z < GetTilemapAsset()->Floors; ++z)
					{
						if (x < CachedTilemapSize[0] && y < CachedTilemapSize[1] && z < CachedTilemapSize[2])
						{
							NewBlocks[x + y * GetTilemapAsset()->LevelSizeX + z * GetTilemapAsset()->LevelSizeX *
								GetTilemapAsset()->LevelSizeY] = GetTilemapAsset()->Blocks[x + y * CachedTilemapSize[0]
								+ z * CachedTilemapSize[0] * CachedTilemapSize[1]];
						}
					}
				}
			}
			CachedTilemapSize[0] = GetTilemapAsset()->LevelSizeX;
			CachedTilemapSize[1] = GetTilemapAsset()->LevelSizeY;
			CachedTilemapSize[2] = GetTilemapAsset()->Floors;
			GetTilemapAsset()->Blocks = NewBlocks;
			FTilemap3DTerrainGenerator::Setup(GetTilemapAsset(), TerrainMesh, TerrainMat, this);
		}

		// 绘制编辑区域
		DrawGrid(-1 * FVector(GetTilemapAsset()->GridSize / 2.0f, GetTilemapAsset()->GridSize / 2.0f, 0.0f),
		         GetTilemapAsset()->LevelSizeX, GetTilemapAsset()->LevelSizeY,
		         GetTilemapAsset()->GridSize, GetCurrentFloor() * GetTilemapAsset()->GridSize,
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
		CollisionLocation.Z += GetCurrentFloor() * GetTilemapAsset()->GridSize + 0.01f;
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
	FTilemap3DTerrainGenerator::ClearVoxel(GetTilemapAsset(), TerrainMesh, TerrainMat);
}

void FTilemap3DEditorViewportClient::OnTilemapGeneratePathFinding()
{
	FTilemap3DPathfindingGenerator::Setup(GetWorld(), GetTilemapAsset(), CurrentTileSet);

	DisplayPathFinding();
}

void FTilemap3DEditorViewportClient::DisplayPathFinding()
{
	{
		for (UTextRenderComponent* TextRenderComp : PathFindingIndexTextArray)
		{
			TextRenderComp->DestroyComponent();
			PreviewScene->RemoveComponent(TextRenderComp);
		}
		PathFindingIndexTextArray.Empty();

		const UTilemapAsset* TilemapAsset = GetTilemapAsset();
		for (int32 Index = 0; Index < TilemapAsset->PathFindingBlocks.Num(); ++Index)
		{
			const FTilemapPathFindingBlock& PathFindingBlock = TilemapAsset->PathFindingBlocks[Index];
			if (UKismetMathLibrary::NotEqual_VectorVector(PathFindingBlock.Location, FVector::Zero()), 1.0f)
			{
				FTransform Transform;
				Transform.SetLocation({
					PathFindingBlock.Location.X - 10.0f, PathFindingBlock.Location.Y + 10.0f,
					PathFindingBlock.Location.Z + 10.0f
				});
				Transform.SetRotation(FQuat({90.0f, 0.0f, 270.0f}));
				Transform.SetScale3D(FVector::One());
				UTextRenderComponent* TextRender = NewObject<UTextRenderComponent>();
				PreviewScene->AddComponent(TextRender, Transform);
				TextRender->SetText(FText::AsNumber(Index));
				PathFindingIndexTextArray.Add(TextRender);
			}
		}
	}

	{
		for (UTextRenderComponent* TextRenderComp : PathFindingEdgeCostTextArray)
		{
			TextRenderComp->DestroyComponent();
			PreviewScene->RemoveComponent(TextRenderComp);
		}
		PathFindingEdgeCostTextArray.Empty();

		const UTilemapAsset* TilemapAsset = GetTilemapAsset();
		for (int32 Index = 0; Index < TilemapAsset->PathFindingBlocks.Num(); ++Index)
		{
			const FTilemapPathFindingBlock& PathFindingBlock = TilemapAsset->PathFindingBlocks[Index];
			if (UKismetMathLibrary::NotEqual_VectorVector(PathFindingBlock.Location, FVector::Zero()), 1.0f)
			{
				for (const auto& Edge : PathFindingBlock.EdgeArray)
				{
					const FVector& EdgeLocation = TilemapAsset->PathFindingBlocks[Edge.Index].Location;
					FTransform Transform;
					Transform.SetLocation({
						(EdgeLocation.X + PathFindingBlock.Location.X + PathFindingBlock.Location.X) / 3.0f,
						(EdgeLocation.Y + PathFindingBlock.Location.Y + PathFindingBlock.Location.Y) / 3.0f,
						PathFindingBlock.Location.Z + 10.0f
					});
					Transform.SetScale3D(FVector::One());
					Transform.SetRotation(FQuat({
						90.0f, UKismetMathLibrary::FindLookAtRotation(PathFindingBlock.Location, EdgeLocation).Yaw, 0.0f
					}));
					UTextRenderComponent* TextRender = NewObject<UTextRenderComponent>();
					PreviewScene->AddComponent(TextRender, Transform);
					TextRender->SetText(FText::AsNumber(Edge.Cost));
					PathFindingEdgeCostTextArray.Add(TextRender);
				}
			}
		}
	}
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

void FTilemap3DEditorViewportClient::OnTilemapEditModeChanged(const ETilemap3DEditMode OldEditMode,
	const ETilemap3DEditMode NewEditMode)
{
	StateMachine->ChangeState(NewEditMode);
}

void FTilemap3DEditorViewportClient::OnTilemapFillFloor()
{
	const auto* TilemapAsset = GetTilemapAsset();
	for (int x = 0; x < TilemapAsset->LevelSizeX; ++x)
	{
		for (int y = 0; y < TilemapAsset->LevelSizeY; ++y)
		{
			
		}
	}
}
