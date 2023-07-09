#pragma once
#include "Tilemap/TilemapAsset.h"
class UProceduralMeshComponent;

class FTilemap3DTerrainGenerate
{
public:
	static void Setup(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent, UMaterialInterface* Material);
	static void ModifyVoxel(UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                        const FVector& Position, const EBlock Block, const int32 Floor,
	                        UMaterialInterface* Material);
	static int32 VectorToIndex(UTilemapAsset* TilemapAsset, const FVector& Location, int32 Floor);

private:
	static bool Check(UTilemapAsset* TilemapAsset, const FVector& Position);
	static void CreateFace(UTilemapAsset* TilemapAsset, const ETilemapDirection Direction,
	                       const FVector& Position);
	static TArray<FVector> GetFaceVertices(const ETilemapDirection Direction, const FVector& Position);
	static FVector GetPositionInDirection(const ETilemapDirection Direction, const FVector& Position);
	static FVector GetNormal(const ETilemapDirection Direction);
	static void ModifyVoxelData(UTilemapAsset* TilemapAsset, const FVector& Position, const EBlock Block,
	                            const int32 Floor);
	static int32 GetTextureIndex(const EBlock Block, const FVector& Normal);


	static void ClearMesh(UTilemapAsset* TilemapAsset);
	static void GenerateMesh(UTilemapAsset* TilemapAsset);
	static void ApplyMesh(const UTilemapAsset* TilemapAsset, UProceduralMeshComponent* MeshComponent,
	                      UMaterialInterface* Material);

private:
	inline static const FVector BlockVertexData[8] = {
		FVector(50, 50, 50),
		FVector(50, -50, 50),
		FVector(50, -50, -50),
		FVector(50, 50, -50),
		FVector(-50, -50, 50),
		FVector(-50, 50, 50),
		FVector(-50, 50, -50),
		FVector(-50, -50, -50)
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
