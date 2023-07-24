#pragma once
#include "Tilemap/TileSet3DAsset.h"
#include "TilemapEditor/Tilemap3DEditorManager.h"

class UTilemapAsset;

struct FAddTileMeshParams
{
	UTilemapAsset* InTilemapAsset;
	FPreviewScene* InPreviewScene;
	FTileMeshMap& InTileMeshMap;
	const FName& ID;
	UStaticMesh* InMesh;
	const FTransform& Transform;
	int32 BlockIndex;
	UMaterialInterface* InMaterial;
};

class FTilemap3DTileMeshGenerator
{
public:
	static void Setup(UTilemapAsset* InTilemapAsset, FTileMeshMap& TileMeshMap, FPreviewScene* PreviewScene, const UTileSet3DAsset* TileSet3DAsset);

	static int32 AddTileMesh(const FAddTileMeshParams& Params);
};
