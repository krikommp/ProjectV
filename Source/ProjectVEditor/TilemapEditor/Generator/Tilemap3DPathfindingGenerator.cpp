#include "Tilemap3DPathfindingGenerator.h"

#include "Tilemap/TilemapAsset.h"
#include "Tilemap/TileSet3DAsset.h"

void FTilemap3DPathfindingGenerator::Setup(UTilemapAsset* InTilemapAsset, const UTileSet3DAsset* InTileSetAsset)
{
	for (int32 Index = 0; Index < InTilemapAsset->Blocks.Num(); ++Index)
	{
		InTilemapAsset->Blocks[Index].Location = IndexToVector(InTilemapAsset, Index);
		PopulateEdgeTileAndCost(InTilemapAsset, Index, InTileSetAsset->bDiagonalMovement);
	}
}

FVector FTilemap3DPathfindingGenerator::IndexToVector(const UTilemapAsset* InTilemapAsset, int32 Index)
{
	const int32 ResidueX = Index % InTilemapAsset->LevelSizeX; 
	const float X = InTilemapAsset->GridSize * ResidueX;

	const int32 ResidueYZ = Index / InTilemapAsset->LevelSizeX;
	const int32 SquaredXY = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY; 
	const int32 ResidueZ = Index / SquaredXY; 
	const int32 ResidueXZ = InTilemapAsset->LevelSizeX * ResidueZ;
	const int32 ResidueY = ResidueYZ - ResidueXZ; 
	const float Y = ResidueY * InTilemapAsset->GridSize;

	const float Z = ResidueZ * InTilemapAsset->HeightSize;

	return FVector(X, Y, Z);
}

void FTilemap3DPathfindingGenerator::PopulateEdgeTileAndCost(UTilemapAsset* InTilemapAsset, int32 Index,
	bool bDiagonalMovement)
{
	TArray<int32> TempIntArray;
	TArray<FTilemapMovementStep> TempMoveStep;
	if (bDiagonalMovement)
	{
		const int32 North = Index - InTilemapAsset->LevelSizeX;
		const int32 East = Index + 1;
		const int32 South = Index + InTilemapAsset->LevelSizeX;
		const int32 West = Index - 1;
		const int32 NorthEast = Index - InTilemapAsset->LevelSizeX + 1;
		const int32 SouthEast = Index + InTilemapAsset->LevelSizeX + 1;
		const int32 SouthWest = Index + InTilemapAsset->LevelSizeX - 1;
		const int32 NorthWest = Index - InTilemapAsset->LevelSizeX - 1;
		TempIntArray = {North, East, South, West, NorthEast, SouthEast, SouthWest, NorthWest};
		TempMoveStep = {
			{North, 1},
			{East, 1},
			{South, 1},
			{West, 1},
			{NorthEast, 1},
			{SouthEast, 1},
			{SouthWest, 1},
			{NorthWest, 1}
		};
	}else
	{
		const int32 North = Index - InTilemapAsset->LevelSizeX;
		const int32 East = Index + 1;
		const int32 South = Index + InTilemapAsset->LevelSizeX;
		const int32 West = Index - 1;
		TempIntArray = {North, East, South, West};
		TempMoveStep = {{North, 1}, {East, 1}, {South, 1}, {West, 1}};
	}
	InTilemapAsset->Blocks[Index].EdgeArrayIndex = TempIntArray;
	InTilemapAsset->Blocks[Index].EdgeArray = TempMoveStep;
}
