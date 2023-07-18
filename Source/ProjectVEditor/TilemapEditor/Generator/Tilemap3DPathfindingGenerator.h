#pragma once

class UTileSet3DAsset;
class UTilemapAsset;

class FTilemap3DPathfindingGenerator
{
public:
	static void Setup(const UObject* WorldContextObject, UTilemapAsset* InTilemapAsset, const UTileSet3DAsset* InTileSetAsset);

private:
	static FVector IndexToVector(const UTilemapAsset* InTilemapAsset, int32 Index);
	static void PopulateEdgeTileAndCost(UTilemapAsset* InTilemapAsset, int32 Index, bool bDiagonalMovement);
	static void ComputeTilemapSurfaceHeight(const UObject* WorldContextObject, UTilemapAsset* InTilemapAsset, int32 Index);
	static void AddMultiLevelGridAtIndex(const UObject* WorldContextObject, UTilemapAsset* InTilemapAsset, int32 InIndex,  const FVector& StartLocation);

	static void CreateMultiLevelGrids(UTilemapAsset* InTilemapAsset, bool bDiagonalMovement);
	static TArray<int32> GetAdjacentIndexes(const UTilemapAsset* InTilemapAsset, int32 Index, bool bDiagonalMovement);

	static void CreateWallsOnGridEdges(UTilemapAsset* InTilemapAsset);
};
