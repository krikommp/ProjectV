// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapPathFinding.h"

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
 * EBlockDirection
 *
 * Block方向
 */
UENUM(BlueprintType)
enum class EBlockDirection : uint8
{
	Forward = 0,
	Right = 1,
	Back = 2,
	Left = 3,
	Up = 4,
	Down = 5
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FBlock
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName BlockID = FName();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EBlock Type = EBlock::Air;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bMarked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location = FVector::Zero();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTilemapPathFinding CanMoveTo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> EdgeArrayIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FTilemapMovementStep> EdgeArray;
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTilemap3DTerrainMeshData
{
	GENERATED_BODY();

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FColor> Colors;
	TArray<FVector2D> UV0;
	int32 VertexCount = 0;

	void Clear();
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
