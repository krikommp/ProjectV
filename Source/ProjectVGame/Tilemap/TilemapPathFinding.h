// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TilemapPathFinding.generated.h"

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTilemapPathFinding
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	int32 Cost = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	int32 Parent = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTilemapMovementStep
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Cost = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FTilemapPathFindingBlock 
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location = FVector::Zero();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> EdgeArrayIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FTilemapMovementStep> EdgeArray;
};
