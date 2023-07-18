﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TilemapAsset.h"
#include "UObject/Object.h"
#include "TileSet3DAsset.generated.h"

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTileSet3DSubObject
{
	GENERATED_BODY()

	FTileSet3DSubObject();

	static FTileSet3DSubObject EmptyBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBlock BlockType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UTexture2D>> BlockTextures;
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
	TArray<FTileSet3DSubObject> TileSets;

	UPROPERTY(EditAnywhere)
	bool bDiagonalMovement = false;
};
