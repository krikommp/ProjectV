﻿#pragma once

class UTilemapAsset;
class FTilemap3DEditorViewportClient;

class FTilemap3DPlayerChessStartGenerator
{
public:
	static void Setup(FTilemap3DEditorViewportClient* ViewportClient, UTilemapAsset* InTilemapAsset);

	static void Remove(UTilemapAsset* InTilemapAsset, const int32 Index);
};
