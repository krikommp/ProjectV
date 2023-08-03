// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapPathFinding.h"

#include "TilemapAsset.generated.h"

class UGridChessData;
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

UCLASS(BlueprintType)
class PROJECTVGAME_API UBlock : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName BlockID = FName();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EBlock Type = EBlock::Air;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName MeshIndex = FName();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform MeshTransform = FTransform::Identity;

	UPROPERTY(Transient)
	int32 MeshInstancedIndex = INDEX_NONE;

	UPROPERTY(Transient)
	TObjectPtr<AActor> ChessInEditor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UGridChessData> ChessData;
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTilemap3DTerrainMeshData
{
	GENERATED_BODY();

	UPROPERTY()
	TArray<FVector> Vertices;
	UPROPERTY()
	TArray<int32> Triangles;
	UPROPERTY()
	TArray<FVector> Normals;
	UPROPERTY()
	TArray<FColor> Colors;
	UPROPERTY()
	TArray<FVector2D> UV0;
	UPROPERTY()
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

	UPROPERTY(VisibleAnywhere, Category="Path Finding")
	int32 LevelSizeZ = 1;

	UPROPERTY(EditAnywhere, Category="Grid Values")
	int32 GridSize = 100;

	UPROPERTY(EditAnywhere, Category="Path Finding")
	int32 HeightSlowIncrement = 100;
	
	UPROPERTY(EditAnywhere, Category="Path Finding")
	int32 HeightBetweenLevel = 200;

	UPROPERTY(EditAnywhere, Category="Path Finding")
	int32 TraceForWallsHeight = 100;

	UPROPERTY(EditAnywhere, Category="Tile Values")
	TObjectPtr<class UTileSet3DAsset> TileSet;

	UPROPERTY()
	TArray<TObjectPtr<UBlock>> Blocks;

	UPROPERTY()
	TArray<FTilemapPathFindingBlock> PathFindingBlocks;

	UPROPERTY()
	FTilemap3DTerrainMeshData MeshData;

	UPROPERTY(VisibleAnywhere, Category="Level Values")
	FTransform PlayerStart;

	UPROPERTY(VisibleAnywhere, Category="Level Values")
	float PlayerStartHeight = 100.0f;

	UPROPERTY(VisibleAnywhere, Category="Level Values")
	float MapBoundOffset = 50.0f;

	UPROPERTY(EditAnywhere, Category="Path Finding")
	TObjectPtr<UMaterialInterface> MoveRangeMat;

	int32 GetMaxLevelHeight() const { return (Floors + 1) * GridSize; }
	int32 GetMinLevelHeight() const { return 0; }
	int32 GetBlockIndex(const int32 X, const int32 Y, const int32 Z) const;

	int32 VectorToIndex(const FVector& Location, int32 Floor) const;
	int32 VectorToIndex(const FVector& Location) const;
	int32 PathFindingBlockToBlock(int32 Index) const;
	int32 PathFindingBlockToBlock(const FTilemapPathFindingBlock& PathFindingBlock) const;

	FVector IndexToVector(int32 Index) const;

	int32 GetEdgeCostFromZDifferent(int32 SelfIndex, int32 OtherIndex) const;

	void AddEdgeBothWays(int32 Index, int32 EdgeIndex, int32 Cost);
	void AddEdge(int32 Index, int32 EdgeIndex, int32 Cost);

	void RemoveEdgeBothWays(int32 Index, int32 EdgeIndex);
	void RemoveCornerEdgeBothWays(int32 Index, int32 EdgeIndex);
	void RemoveEdge(int32 Index, int32 EdgeIndex);
	void RemoveInValidEdge(int32 Index);

	void SetEdgeCost(int32 Index, int32 EdgeIndex, int32 Cost);
};
