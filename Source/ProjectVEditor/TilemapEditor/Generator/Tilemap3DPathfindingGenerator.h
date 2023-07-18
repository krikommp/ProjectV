#pragma once

class UTileSet3DAsset;
class UTilemapAsset;

class FTilemap3DPathfindingGenerator
{
public:
	static void Setup(UTilemapAsset* InTilemapAsset, const UTileSet3DAsset* InTileSetAsset);

private:
	static FVector IndexToVector(const UTilemapAsset* InTilemapAsset, int32 Index);
	static void PopulateEdgeTileAndCost(UTilemapAsset* InTilemapAsset, int32 Index, bool bDiagonalMovement);
};
