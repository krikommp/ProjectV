// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Team/TeamType.h"
#include "ChessBlueprintLibrary.generated.h"

class UGridHeroInfo;
class ATilemap3DActor;
class AGridChessBase;
class UGridChessData;
struct FTilemapPathFindingBlock;

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

	// 将一个棋盘上的棋子移除
	UFUNCTION(BlueprintCallable, Category=Chess, meta=(WorldContext = "WorldContextObject"))
	static void DestroyChessOnTilemap(const UObject* WorldContextObject, AGridChessBase* Chess, const ETeamType TeamType);
	
	// 判断该索引位置上是否可以放置一个玩家棋子
	UFUNCTION(BlueprintCallable, Category=Chess, meta=(WorldContext = "WorldContextObject"))
	static bool CheckIndexInPlayerStartRange(const UObject* WorldContextObject, const int32 PathfindingIndex);

	// 获取玩家放置棋子范围
	UFUNCTION(BlueprintCallable, Category=Chess, meta=(WorldContext = "WorldContextObject"))
	static TArray<FTilemapPathFindingBlock> GetPlayerStartLocations(const UObject* WorldContextObject);

	// 场景中放置Decl
	UFUNCTION(BlueprintCallable, Category=Chess, meta=(WorldContext = "WorldContextObject"))
	static UDecalComponent* SpawnDecalOnTilemap(const UObject* WorldContextObject, const FVector& Location, UMaterialInterface* Mat, const FLinearColor& Color);
	
	static AGridChessBase* SpawnChessOnTilemap(const UObject* WorldContextObject, int32 PathfindingIndex, UGridChessData* ChessData,
	                                           UGridHeroInfo* ChessInfo, ATilemap3DActor* Tilemap3DActor);
};
