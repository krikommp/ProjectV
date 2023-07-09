#include "Tilemap3DTerrainGenerate.h"

#include "ProceduralMeshComponent.h"

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

void FTilemap3DTerrainGenerate::Setup(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	UMaterialInterface* Material)
{
	ClearMesh(TilemapAsset);
	GenerateMesh(TilemapAsset);
	ApplyMesh(TilemapAsset, MeshComponent, Material);
}

void FTilemap3DTerrainGenerate::ModifyVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                            const FVector& Position,
                                            const EBlock Block, const int32 Floor, UMaterialInterface* Material)
{
	ModifyVoxelData(TilemapAsset, Position, Block, Floor);
	ClearMesh(TilemapAsset);
	GenerateMesh(TilemapAsset);
	ApplyMesh(TilemapAsset, MeshComponent, Material);
}

void FTilemap3DTerrainGenerate::ModifyVoxelData(UTilemapAsset* TilemapAsset, const FVector& Position,
                                                const EBlock Block, const int32 Floor)
{
	const int32 Index = VectorToIndex(TilemapAsset, Position, Floor);

	TilemapAsset->Blocks[Index].Type = Block;
}

int32 FTilemap3DTerrainGenerate::GetTextureIndex(const EBlock Block, const FVector& Normal)
{
	if (Normal == FVector::UpVector)
		return 0;
	if (Normal == FVector::DownVector)
		return 2;
	return 1;
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
							CreateFace(TilemapAsset, Direction, Position * 100);
						}
					}
					TilemapAsset->Blocks[Index].bMarked = true;
				}
			}
		}
	}
}

void FTilemap3DTerrainGenerate::ApplyMesh(const UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                          UMaterialInterface* Material)
{
	MeshComponent->SetMaterial(0, Material);
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
                                           const FVector& Position)
{
	const auto Normal = GetNormal(Direction);
	const auto Color = FColor(0, 0, 0, GetTextureIndex(EBlock::Cube, Normal));

	TilemapAsset->MeshData.Vertices.Append(GetFaceVertices(Direction, Position));
	TilemapAsset->MeshData.Triangles.Append({
		TilemapAsset->MeshData.VertexCount + 3, TilemapAsset->MeshData.VertexCount + 2,
		TilemapAsset->MeshData.VertexCount, TilemapAsset->MeshData.VertexCount + 2,
		TilemapAsset->MeshData.VertexCount + 1, TilemapAsset->MeshData.VertexCount
	});
	TilemapAsset->MeshData.Normals.Append({Normal, Normal, Normal, Normal});
	TilemapAsset->MeshData.Colors.Append({Color, Color, Color, Color});
	TilemapAsset->MeshData.UV0.Append({FVector2D(0, 0), FVector2D(1, 0), FVector2D(1, 1), FVector2D(0, 1)});

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
