// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TilemapAsset.generated.h"

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

	UPROPERTY(EditAnywhere, Category="Level Values")
	int32 LevelWidth = 0;

	UPROPERTY(EditAnywhere, Category="Level Values")
	int32 LevelDepth = 0;

	UPROPERTY(EditAnywhere, Category="Grid Values")
	int32 FloorsHeight = 1;

	UPROPERTY(EditAnywhere, Category="Grid Values")
	int32 GridSize = 1;
};
