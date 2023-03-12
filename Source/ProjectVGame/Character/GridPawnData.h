// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridPawnData.generated.h"

class UGridAbilitySet;
class UGridInputConfig;

/**
 * UGridPawnData
 *
 * 用于定义 Pawn 的不可变游戏资产
 */
UCLASS(BlueprintType, Const, meta=(DisplayName = "Grid Pawn Data", ShortTooltip="用于定义 Pawn 的不可变游戏资产"))
class PROJECTVGAME_API UGridPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGridPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// 需要被实例化的 Pawn 类实例
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Abilities")
	TArray<TObjectPtr<UGridAbilitySet>> AbilitySets;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	TObjectPtr<UGridInputConfig> InputConfig;
};
