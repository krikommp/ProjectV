// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridChessData.generated.h"

class AGridChessBase;
/**
 * UGridChessData
 *
 * 用于定义 Chess 的不可变资产
 */
UCLASS(BlueprintType, Const, meta=(DisplayName = "Grid Chess Piece Data", ShortTooltip="用于定义 Piece 的不可变游戏资产"))
class PROJECTVGAME_API UGridChessData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 所属 Piece ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chess)
	FName ChessID = NAME_None;

	// 本地玩家索引，临时
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Chess)
	int32 PlayerIndex = 0;

	// 位置
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chess)
	FTransform ChessTransform;

	// 需要被实例化的棋子
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Chess)
	TSubclassOf<AGridChessBase> ChessClass;
};
