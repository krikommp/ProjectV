#pragma once

class UTileSet3DAsset;
class FTilemap3DEditorViewportClient;
class UTilemapAsset;

class FTilemap3DChessGenerator
{
public:
	static void Setup(FTilemap3DEditorViewportClient* ViewportClient, UTilemapAsset* InTilemapAsset, const UTileSet3DAsset* InTileSetAsset);
};
