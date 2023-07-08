// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TilemapAsset.generated.h"

/**
 * EBlock
 *
 * Block 类型
 */
UENUM(BlueprintType)
enum class EBlock : uint8
{
	Air = 0,
	Cube = 1,
	Mesh = 2,
	Unknown UMETA(Hidden)
};

/**
 * ETilemapDirection
 *
 * 方向
 */
UENUM(BlueprintType)
enum class ETilemapDirection : uint8
{
	Forward = 0,
	Right = 1,
	Back = 2,
	Left = 3,
	Up = 4,
	Down = 5
};

USTRUCT(BlueprintType)
struct FBlock
{
	GENERATED_BODY()

	EBlock Type = EBlock::Air;
	bool bMarked = false;
};

USTRUCT(BlueprintType)
struct FTilemap3DTerrainMeshData
{
	GENERATED_BODY();

public:
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FColor> Colors;
	TArray<FVector2D> UV0;
	int32 VertexCount = 0;

	void Clear()
	{
		Vertices.Empty();
		Triangles.Empty();
		Normals.Empty();
		Colors.Empty();
		UV0.Empty();
		VertexCount = 0;
	}

};

/**
 * UTilemapAsset
 *
 * 3d tilemap 资产
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTVGAME_API UTilemapAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Level Values")
	int32 Floors = 0;

	UPROPERTY(EditAnywhere, Category="Level Values", meta=(DisplayName="LevelWidth"))
	int32 LevelSizeX = 0;

	UPROPERTY(EditAnywhere, Category="Level Values", meta=(DisplayName="LevelDepth"))
	int32 LevelSizeY = 0;

	UPROPERTY(EditAnywhere, Category="Grid Values")
	int32 GridSize = 100;

	UPROPERTY(EditAnywhere, Category="Grid Values")
	int32 HeightSize = 100;

	UPROPERTY()
	TArray<FBlock> Blocks;

	UPROPERTY()
	FTilemap3DTerrainMeshData MeshData;

	int32 GetMaxLevelHeight() const { return Floors * HeightSize; }
	int32 GetMinLevelHeight() const { return 0; }

	int32 GetBlockIndex(const int32 X, const int32 Y, const int32 Z) const
	{
		return Z * LevelSizeX * LevelSizeY + Y * LevelSizeX + X;
	}
};
