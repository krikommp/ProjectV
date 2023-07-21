#pragma once
#include "Tilemap/TilemapAsset.h"
#include "Tilemap/TileSet3DAsset.h"
class UProceduralMeshComponent;
class FTilemap3DEditorViewportClient;

class FTilemap3DTerrainGenerator
{
public:
	static void Setup(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                  UMaterialInterface* Material, FTilemap3DEditorViewportClient* ViewClient);
	static void ModifyVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                        const FVector& Position, const FTileSet3DCube& Block, const int32 Floor,
	                        UMaterialInterface* Material, FTilemap3DEditorViewportClient* ViewClient);
	static void ClearVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                       UMaterialInterface* Material);
private:
	static bool Check(UTilemapAsset* TilemapAsset, const FVector& Position);
	static void CreateFace(UTilemapAsset* TilemapAsset, const EBlockDirection Direction,
	                       const FVector& Position, const FBlock& Block, FTilemap3DEditorViewportClient* ViewClient);
	static TArray<FVector> GetFaceVertices(const EBlockDirection Direction, const FVector& Position);
	static FVector GetPositionInDirection(const EBlockDirection Direction, const FVector& Position);
	static FVector GetNormal(const EBlockDirection Direction);
	static void ModifyVoxelData(UTilemapAsset* TilemapAsset, const FVector& Position, const FTileSet3DCube& Block,
	                            const int32 Floor);
	static int32 GetTextureIndex(const FBlock& Block, const FVector& Normal,
	                             FTilemap3DEditorViewportClient* ViewClient);


	static void ClearMesh(UTilemapAsset* TilemapAsset);
	static void GenerateMesh(UTilemapAsset* TilemapAsset, FTilemap3DEditorViewportClient* ViewClient);
	static void ApplyMesh(const UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                      UMaterialInterface* Material);

private:
	inline static const FVector BlockVertexData[8] = {
		FVector(50, 50, 100),
		FVector(50, -50, 100),
		FVector(50, -50, 0),
		FVector(50, 50, 0),
		FVector(-50, -50, 100),
		FVector(-50, 50, 100),
		FVector(-50, 50, 0),
		FVector(-50, -50, 0)
	};

	inline static const int BlockTriangleData[24] = {
		0, 1, 2, 3, // Forward
		5, 0, 3, 6, // Right
		4, 5, 6, 7, // Back
		1, 4, 7, 2, // Left
		5, 4, 1, 0, // Up
		3, 2, 7, 6 // Down
	};
};
