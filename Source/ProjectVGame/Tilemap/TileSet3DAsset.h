// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapAsset.h"
#include "UObject/Object.h"
#include "Heros/GridHeroData.h"
#include "TileSet3DAsset.generated.h"

class UGridChessData;
class UDataTable;

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTileSet3DCube
{
	GENERATED_BODY()

	FTileSet3DCube();

	static FTileSet3DCube EmptyBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBlock BlockType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UTexture2D>> BlockTextures;
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTileSet3DMesh
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Material;
};

/**
 * UTileSet3DAsset
 *
 * TileSet 
 */
UCLASS(BlueprintType, Const, meta=(DisplayName = "3D TileSet", ShortTooltip="Define 3D TileSet Asset"))
class PROJECTVGAME_API UTileSet3DAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2DArray> Textures;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere)
	TArray<FTileSet3DCube> TileCubeSets;

	UPROPERTY(EditAnywhere)
	TArray<FTileSet3DMesh> TileMeshSets;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> ChessData;

	UPROPERTY(Transient)
	TMap<FName, FGridHeroData> ChessMap;

	UPROPERTY(EditAnywhere)
	bool bDiagonalMovement = false;
};
