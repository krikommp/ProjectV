#include "Tilemap3DEditorViewportClient.h"

#include "GridTraceChannel.h"
#include "Components/BoxComponent.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Tilemap3DEditorManager.h"
#include "Tilemap3DSelected.h"
#include "TilemapEditor/Tilemap3DEditorSettings.h"

class FTilemap3DTerrainGenerate
{
public:
	static void ModifyVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                        const FVector& Position, const EBlock Block, const int32 Floor);
	static int32 VectorToIndex(UTilemapAsset* TilemapAsset, const FVector& Location, int32 Floor);

private:
	static bool Check(UTilemapAsset* TilemapAsset, const FVector& Position);
	static void CreateFace(UTilemapAsset* TilemapAsset, const ETilemapDirection Direction,
	                       const FVector& Position, const FColor& Color);
	static TArray<FVector> GetFaceVertices(const ETilemapDirection Direction, const FVector& Position);
	static FVector GetPositionInDirection(const ETilemapDirection Direction, const FVector& Position);
	static FVector GetNormal(const ETilemapDirection Direction);
	static void ModifyVoxelData(UTilemapAsset* TilemapAsset, const FVector& Position, const EBlock Block,
	                            const int32 Floor);


	static void ClearMesh(UTilemapAsset* TilemapAsset);
	static void GenerateMesh(UTilemapAsset* TilemapAsset);
	static void ApplyMesh(const UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent);

private:
	inline static const FVector BlockVertexData[8] = {
		FVector(50, 50, 50),
		FVector(50, -50, 50),
		FVector(50, -50, -50),
		FVector(50, 50, -50),
		FVector(-50, -50, 50),
		FVector(-50, 50, 50),
		FVector(-50, 50, -50),
		FVector(-50, -50, -50)
	};

	inline static const int BlockTriangleData[24] = {
		0, 1, 2, 3, // Forward
		5, 0, 3, 6, // Right
		4, 5, 6, 7, // Back
		1, 4, 7, 2, // Left
		5, 4, 1, 0, // Up
		3, 2, 7, 6 // Down
	};
};

int32 FTilemap3DTerrainGenerate::VectorToIndex(UTilemapAsset* TilemapAsset, const FVector& Location, int32 Floor)
{
	const float PivotX = (TilemapAsset->GridSize * 0.5) + Location.X;
	const float PivotY = (TilemapAsset->GridSize * 0.5) + Location.Y;

	const float ModX = FMath::Floor(FMath::Fmod(PivotX, TilemapAsset->GridSize));
	const float ModY = FMath::Floor(FMath::Fmod(PivotY, TilemapAsset->GridSize));

	const int AddX = ModX ? 1 : 0;
	const int AddY = ModY ? 1 : 0;

	const int X = FMath::Floor(((PivotX + AddX)) / TilemapAsset->GridSize);
	const int Y = FMath::Floor(((PivotY + AddY)) / TilemapAsset->GridSize) *
		TilemapAsset->LevelSizeX;

	const int32 Result = X + Y;

	return Result + TilemapAsset->LevelSizeX * TilemapAsset->LevelSizeY * Floor;
}

void FTilemap3DTerrainGenerate::ModifyVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                            const FVector& Position,
                                            const EBlock Block, const int32 Floor)
{
	ModifyVoxelData(TilemapAsset, Position, Block, Floor);
	ClearMesh(TilemapAsset);
	GenerateMesh(TilemapAsset);
	ApplyMesh(TilemapAsset, MeshComponent);
}

void FTilemap3DTerrainGenerate::ModifyVoxelData(UTilemapAsset* TilemapAsset, const FVector& Position,
                                                const EBlock Block, const int32 Floor)
{
	const int32 Index = VectorToIndex(TilemapAsset, Position, Floor);

	TilemapAsset->Blocks[Index].Type = Block;
}

void FTilemap3DTerrainGenerate::ClearMesh(UTilemapAsset* TilemapAsset)
{
	TilemapAsset->MeshData.Clear();
}

void FTilemap3DTerrainGenerate::GenerateMesh(UTilemapAsset* TilemapAsset)
{
	for (int32 x = 0; x < TilemapAsset->LevelSizeX; ++x)
	{
		for (int32 y = 0; y < TilemapAsset->LevelSizeY; ++y)
		{
			for (int32 z = 0; z < TilemapAsset->Floors; ++z)
			{
				const int32 Index = TilemapAsset->GetBlockIndex(x, y, z);
				if (TilemapAsset->Blocks[Index].Type != EBlock::Air)
				{
					const auto Position = FVector(x, y, z);
					for (const auto Direction : {
						     ETilemapDirection::Forward, ETilemapDirection::Right, ETilemapDirection::Back,
						     ETilemapDirection::Left, ETilemapDirection::Up, ETilemapDirection::Down
					     })
					{
						if (Check(TilemapAsset, GetPositionInDirection(Direction, Position)))
						{
							CreateFace(TilemapAsset, Direction, Position * 100, FColor::Red);
						}
					}
					TilemapAsset->Blocks[Index].bMarked = true;
				}
			}
		}
	}
}

void FTilemap3DTerrainGenerate::ApplyMesh(const UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent)
{
	MeshComponent->CreateMeshSection(
		0,
		TilemapAsset->MeshData.Vertices,
		TilemapAsset->MeshData.Triangles,
		TilemapAsset->MeshData.Normals,
		TilemapAsset->MeshData.UV0,
		TilemapAsset->MeshData.Colors,
		TArray<FProcMeshTangent>(),
		true
	);
}

bool FTilemap3DTerrainGenerate::Check(UTilemapAsset* TilemapAsset, const FVector& Position)
{
	if (Position.X >= TilemapAsset->LevelSizeX || Position.Y >= TilemapAsset->LevelSizeY || Position.X < 0 || Position.Y
		< 0 || Position.Z < 0)
		return true;
	if (Position.Z >= TilemapAsset->Floors) return true;
	return TilemapAsset->Blocks[TilemapAsset->GetBlockIndex(Position.X, Position.Y, Position.Z)].Type == EBlock::Air;
}

void FTilemap3DTerrainGenerate::CreateFace(UTilemapAsset* TilemapAsset, const ETilemapDirection Direction,
                                           const FVector& Position, const FColor& Color)
{
	const auto Normal = GetNormal(Direction);

	TilemapAsset->MeshData.Vertices.Append(GetFaceVertices(Direction, Position));
	TilemapAsset->MeshData.Triangles.Append({
		TilemapAsset->MeshData.VertexCount + 3, TilemapAsset->MeshData.VertexCount + 2,
		TilemapAsset->MeshData.VertexCount, TilemapAsset->MeshData.VertexCount + 2,
		TilemapAsset->MeshData.VertexCount + 1, TilemapAsset->MeshData.VertexCount
	});
	TilemapAsset->MeshData.Normals.Append({Normal, Normal, Normal, Normal});
	TilemapAsset->MeshData.Colors.Append({Color, Color, Color, Color});
	TilemapAsset->MeshData.UV0.Append({FVector2D(1, 1), FVector2D(1, 0), FVector2D(0, 0), FVector2D(0, 1)});

	TilemapAsset->MeshData.VertexCount += 4;
}

TArray<FVector> FTilemap3DTerrainGenerate::GetFaceVertices(const ETilemapDirection Direction, const FVector& Position)
{
	TArray<FVector> Vertices;

	for (int i = 0; i < 4; i++)
	{
		Vertices.Add(BlockVertexData[BlockTriangleData[i + static_cast<int>(Direction) * 4]] + Position);
	}

	return Vertices;
}

FVector FTilemap3DTerrainGenerate::GetPositionInDirection(const ETilemapDirection Direction, const FVector& Position)
{
	switch (Direction)
	{
	case ETilemapDirection::Forward: return Position + FVector::ForwardVector;
	case ETilemapDirection::Back: return Position + FVector::BackwardVector;
	case ETilemapDirection::Left: return Position + FVector::LeftVector;
	case ETilemapDirection::Right: return Position + FVector::RightVector;
	case ETilemapDirection::Up: return Position + FVector::UpVector;
	case ETilemapDirection::Down: return Position + FVector::DownVector;
	default: throw std::invalid_argument("Invalid direction");
	}
}

FVector FTilemap3DTerrainGenerate::GetNormal(const ETilemapDirection Direction)
{
	switch (Direction)
	{
	case ETilemapDirection::Forward: return FVector::ForwardVector;
	case ETilemapDirection::Right: return FVector::RightVector;
	case ETilemapDirection::Back: return FVector::BackwardVector;
	case ETilemapDirection::Left: return FVector::LeftVector;
	case ETilemapDirection::Up: return FVector::UpVector;
	case ETilemapDirection::Down: return FVector::DownVector;
	default: throw std::invalid_argument("Invalid direction");
	}
}

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

	// 创建预览 actor
	TilemapSelectedPreview = FEditorViewportClient::GetWorld()->SpawnActor<ATilemap3DSelected>(ATilemap3DSelected::StaticClass());

	SetViewLocation(FVector(0.f, 100.f, 100.f));
	SetLookAtLocation(FVector::Zero(), true);

	HitResultTraceDistance = 10000.0f;

	const auto OnTilemapEditStatueChangedDelegate =
		FTilemap3DEditDelegates::FOnTilemapEditStatueChanged::FDelegate::CreateRaw(
			this, &FTilemap3DEditorViewportClient::OnTilemapEditStatueChanged);
	FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Add(
		OnTilemapEditStatueChangedDelegate);

	const auto OnTilemapModelChangedDelegate = FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FTilemap3DEditorViewportClient::OnTilemapModelChanged);
	FTilemap3DEditDelegates::OnTilemapModelChanged.Add(
		OnTilemapModelChangedDelegate);
}

FTilemap3DEditorViewportClient::~FTilemap3DEditorViewportClient()
{
	FTilemap3DEditDelegates::OnTilemapEditStatueChanged.RemoveAll(this);
	FTilemap3DEditDelegates::OnTilemapModelChanged.RemoveAll(this);
}

void FTilemap3DEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
}

void FTilemap3DEditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(TilemapSelectedPreview);
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
			FTilemap3DTerrainGenerate::ModifyVoxel(GetTilemapAsset(), TerrainMesh, HitResult.Location, EBlock::Cube,
			                                       GetCurrentFloor());
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
	}
	else
	{
		Clear();
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
