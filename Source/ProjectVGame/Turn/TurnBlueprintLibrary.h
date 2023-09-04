// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Team/TeamType.h"
#include "UObject/Object.h"
#include "TurnBlueprintLibrary.generated.h"

class AGridChessBase;
/**
 * UTurnBlueprintLibrary
 *
 * 回合控制辅助函数
 */
UCLASS()
class PROJECTVGAME_API UTurnBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// 获取当前回合待行动的队伍
	UFUNCTION(BlueprintPure, Category=Turn, meta=(WorldContext = "WorldContextObject"))
	static ETeamType GetCurrentTurnTeam(const UObject* WorldContextObject);

	// 获取当前回合待行动的队伍
	UFUNCTION(BlueprintPure, Category=Turn, meta=(WorldContext = "WorldContextObject"))
	static TArray<AGridChessBase*> GetCurrentTurnChess(const UObject* WorldContextObject);
};
