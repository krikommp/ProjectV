// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridMapStruct.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct PROJECTVGAME_API FStructRange
{
	GENERATED_BODY()

public:
	FStructRange();

	FStructRange(int32 InIndex, int32 InDistance);
public:
	UPROPERTY(BlueprintReadWrite)
	int32 Index;

	UPROPERTY(BlueprintReadWrite)
	int32 Distance;
};

USTRUCT(BlueprintType, Blueprintable)
struct PROJECTVGAME_API FStructPathFinding
{
	GENERATED_BODY()

public:
	FStructPathFinding();

	FStructPathFinding(int32 InIndex, int32 InCost, int32 InParent);

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Index;

	UPROPERTY(BlueprintReadWrite)
	int32 Cost;

	UPROPERTY(BlueprintReadWrite)
	int32 Parent;
};

USTRUCT(BlueprintType, Blueprintable)
struct PROJECTVGAME_API FStructIntArray
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<int32> Index;

public:
	FORCEINLINE int operator[](int InIndex) const
	{
		if (Index.IsValidIndex(InIndex))
		{
			return Index[InIndex];
		}else {
			return int{};
		}
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct PROJECTVGAME_API FStructIndexCost
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Cost = INDEX_NONE;
};

USTRUCT(BlueprintType, Blueprintable)
struct PROJECTVGAME_API FStructTileTags
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};

USTRUCT(BlueprintType, Blueprintable)
struct PROJECTVGAME_API FStructEdgeArray
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FStructIndexCost> Array;

public:
	FORCEINLINE FStructIndexCost operator[](int Index)
	{
		if (Array.IsValidIndex(Index)) {
			return Array[Index];
		}
		else
		{
			return FStructIndexCost{};
		}
	}
};