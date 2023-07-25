#include "Tilemap3DTerrainGenerator.h"
#include "ProjectVEditor.h"
#include "ProceduralMeshComponent.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

void FTilemap3DTerrainGenerator::Setup(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                      UMaterialInterface* Material, FTilemap3DEditorViewportClient* ViewClient)
{
	ClearMesh(TilemapAsset);
	GenerateMesh(TilemapAsset, ViewClient);
	ApplyMesh(TilemapAsset, MeshComponent, Material);
}

void FTilemap3DTerrainGenerator::ModifyVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                            const FVector& Position,
                                            const FTileSet3DCube& Block, const int32 Floor,
                                            UMaterialInterface* Material,
                                            FTilemap3DEditorViewportClient* ViewClient)
{
	ModifyVoxelData(TilemapAsset, Position, Block, Floor);
	ClearMesh(TilemapAsset);
	GenerateMesh(TilemapAsset, ViewClient);
	ApplyMesh(TilemapAsset, MeshComponent, Material);
}

void FTilemap3DTerrainGenerator::ClearVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                           UMaterialInterface* Material)
{
	for (int32 x = 0; x < TilemapAsset->LevelSizeX; ++x)
	{
		for (int32 y = 0; y < TilemapAsset->LevelSizeY; ++y)
		{
			for (int32 z = 0; z < TilemapAsset->Floors; ++z)
			{
				const int32 Index = TilemapAsset->GetBlockIndex(x, y, z);
				TilemapAsset->Blocks[Index].bMarked = false;
				TilemapAsset->Blocks[Index].Type = EBlock::Air;
				TilemapAsset->Blocks[Index].BlockID = FName();
				TilemapAsset->Blocks[Index].Cost = MAX_int32;
			}
		}
	}
	ClearMesh(TilemapAsset);
	ApplyMesh(TilemapAsset, MeshComponent, Material);
}

void FTilemap3DTerrainGenerator::ModifyVoxelData(UTilemapAsset* TilemapAsset, const FVector& Position,
                                                const FTileSet3DCube& Block, const int32 Floor)
{
	const int32 Index = TilemapAsset->VectorToIndex(Position, Floor);

	TilemapAsset->Blocks[Index].Type = Block.BlockType;
	TilemapAsset->Blocks[Index].BlockID = Block.ID;
	TilemapAsset->Blocks[Index].Cost = Block.Cost;
}

int32 FTilemap3DTerrainGenerator::GetTextureIndex(const FBlock& Block, const FVector& Normal,
                                                 FTilemap3DEditorViewportClient* ViewClient)
{
	if (Normal == FVector::UpVector)
		return ViewClient->GetBlockTextureIndex(Block.BlockID, 0);
	if (Normal == FVector::DownVector)
		return ViewClient->GetBlockTextureIndex(Block.BlockID, 2);
	return ViewClient->GetBlockTextureIndex(Block.BlockID, 1);
}

void FTilemap3DTerrainGenerator::ClearMesh(UTilemapAsset* TilemapAsset)
{
	TilemapAsset->MeshData.Clear();
}

void FTilemap3DTerrainGenerator::GenerateMesh(UTilemapAsset* TilemapAsset, FTilemap3DEditorViewportClient* ViewClient)
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
						     EBlockDirection::Forward, EBlockDirection::Right, EBlockDirection::Back,
						     EBlockDirection::Left, EBlockDirection::Up, EBlockDirection::Down
					     })
					{
						if (Check(TilemapAsset, GetPositionInDirection(Direction, Position)))
						{
							CreateFace(TilemapAsset, Direction, Position * 100, TilemapAsset->Blocks[Index],
							           ViewClient);
						}
					}
					TilemapAsset->Blocks[Index].bMarked = true;
				}
			}
		}
	}
}

void FTilemap3DTerrainGenerator::ApplyMesh(const UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
                                          UMaterialInterface* Material)
{
	MeshComponent->SetMaterial(0, Material);
	MeshComponent->SetCollisionResponseToChannel(PathTrace, ECR_Block);
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

bool FTilemap3DTerrainGenerator::Check(UTilemapAsset* TilemapAsset, const FVector& Position)
{
	if (Position.X >= TilemapAsset->LevelSizeX || Position.Y >= TilemapAsset->LevelSizeY || Position.X < 0 || Position.Y
		< 0 || Position.Z < 0)
		return true;
	if (Position.Z >= TilemapAsset->Floors) return true;
	return TilemapAsset->Blocks[TilemapAsset->GetBlockIndex(Position.X, Position.Y, Position.Z)].Type == EBlock::Air;
}

void FTilemap3DTerrainGenerator::CreateFace(UTilemapAsset* TilemapAsset, const EBlockDirection Direction,
                                           const FVector& Position, const FBlock& Block,
                                           FTilemap3DEditorViewportClient* ViewClient)
{
	const auto Normal = GetNormal(Direction);
	const auto Color = FColor(0, 0, 0, GetTextureIndex(Block, Normal, ViewClient));

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

TArray<FVector> FTilemap3DTerrainGenerator::GetFaceVertices(const EBlockDirection Direction, const FVector& Position)
{
	TArray<FVector> Vertices;

	for (int i = 0; i < 4; i++)
	{
		Vertices.Add(BlockVertexData[BlockTriangleData[i + static_cast<int>(Direction) * 4]] + Position);
	}

	return Vertices;
}

FVector FTilemap3DTerrainGenerator::GetPositionInDirection(const EBlockDirection Direction, const FVector& Position)
{
	switch (Direction)
	{
	case EBlockDirection::Forward: return Position + FVector::ForwardVector;
	case EBlockDirection::Back: return Position + FVector::BackwardVector;
	case EBlockDirection::Left: return Position + FVector::LeftVector;
	case EBlockDirection::Right: return Position + FVector::RightVector;
	case EBlockDirection::Up: return Position + FVector::UpVector;
	case EBlockDirection::Down: return Position + FVector::DownVector;
	default: throw std::invalid_argument("Invalid direction");
	}
}

FVector FTilemap3DTerrainGenerator::GetNormal(const EBlockDirection Direction)
{
	switch (Direction)
	{
	case EBlockDirection::Forward: return FVector::ForwardVector;
	case EBlockDirection::Right: return FVector::RightVector;
	case EBlockDirection::Back: return FVector::BackwardVector;
	case EBlockDirection::Left: return FVector::LeftVector;
	case EBlockDirection::Up: return FVector::UpVector;
	case EBlockDirection::Down: return FVector::DownVector;
	default: throw std::invalid_argument("Invalid direction");
	}
}
