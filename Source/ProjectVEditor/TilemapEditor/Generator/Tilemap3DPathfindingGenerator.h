#pragma once

class UTilemapAsset;

class FTilemap3DPathfindingGenerator
{
public:
	static void Setup(UTilemapAsset* InAsset);

private:
	static FVector IndexToVector(const UTilemapAsset* InAsset, int32 Index);
};
