// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridChessPieceData.generated.h"

class AGridChessPiece;
class UGridAbilitySet;
class UGridCard;
class USMInstance;

/**
 * UGridChessPieceData
 *
 * 用于定义 棋子 的不可变资产
 */
UCLASS(BlueprintType, Const, meta=(DisplayName = "Grid Chess Piece Data", ShortTooltip="用于定义 Piece 的不可变游戏资产"))
class PROJECTVGAME_API UGridChessPieceData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UGridChessPieceData(const FObjectInitializer& ObjectInitializer);

public:
	// 需要被实例化的棋子
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|ChessPiece")
	TSubclassOf<AGridChessPiece> ChessPieceClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Abilities")
	TArray<TObjectPtr<UGridAbilitySet>> AbilitySets;

	// 所属 Piece ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid|ChessPiece")
	FName PieceID = FName();

	// 本地玩家索引，临时
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|ChessPiece")
	int32 PlayerIndex = 0;

	// AI状态机资源
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|AIStateMachine")
	TSubclassOf<USMInstance> AIStateMachine;

	// 位置
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid|ChessPiece")
	FTransform ChessTransform;
};
