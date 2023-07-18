#include "Tilemap3DPathfindingGenerator.h"

#include "Tilemap/TilemapAsset.h"

void FTilemap3DPathfindingGenerator::Setup(UTilemapAsset* InAsset)
{
	for (int32 Index = 0; Index < InAsset->Blocks.Num(); ++Index)
	{
		InAsset->Blocks[Index].Location = IndexToVector(InAsset, Index);
	}
}

FVector FTilemap3DPathfindingGenerator::IndexToVector(const UTilemapAsset* InAsset, int32 Index)
{
	const int32 ResidueX = Index % InAsset->LevelSizeX; 
	const float X = InAsset->GridSize * ResidueX;

	const int32 ResidueYZ = Index / InAsset->LevelSizeX;
	const int32 SquaredXY = InAsset->LevelSizeX * InAsset->LevelSizeY; 
	const int32 ResidueZ = Index / SquaredXY; 
	const int32 ResidueXZ = InAsset->LevelSizeX * ResidueZ;
	const int32 ResidueY = ResidueYZ - ResidueXZ; 
	const float Y = ResidueY * InAsset->GridSize;

	const float Z = ResidueZ * InAsset->HeightSize;

	return FVector(X, Y, Z);
}
