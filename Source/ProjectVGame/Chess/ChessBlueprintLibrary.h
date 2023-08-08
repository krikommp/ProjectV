// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ChessBlueprintLibrary.generated.h"

class UGridHeroInfo;
class ATilemap3DActor;
class AGridChessBase;
class UGridChessData;

/**
 * UChessBlueprintLibrary
 *
 * 一系列操作 Chess 的蓝图辅助函数
 */
UCLASS()
class PROJECTVGAME_API UChessBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 在鼠标位置放置一个棋子
	UFUNCTION(BlueprintCallable, Category=Chess, meta=(WorldContext = "WorldContextObject"))
	static AGridChessBase* SpawnChessOnTilemapByCursor(const UObject* WorldContextObject, ETraceTypeQuery TraceChannel, UGridHeroInfo* ChessInfo);
	
	static AGridChessBase* SpawnChessOnTilemap(const UObject* WorldContextObject, int32 PathfindingIndex, UGridChessData* ChessData,
	                                           UGridHeroInfo* ChessInfo, ATilemap3DActor* Tilemap3DActor);
};
