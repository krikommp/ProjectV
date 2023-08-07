// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridSettingDefaultValues.generated.h"

/**
 * UGridSettingDefaultValues
 *
 * 用于设定存档默认值
 */
UCLASS(Config="GridDefaultValues")
class PROJECTVGAME_API UGridSettingDefaultValues : public UObject
{
	GENERATED_BODY()

public:
	// 玩家棋子包存档的默认值
	UPROPERTY(Config, EditAnywhere, Category="Chess|Package")
	TArray<FName> DefaultChessIDs;

	// 玩家名称的默认值
	UPROPERTY(Config, EditAnywhere, Category="Player")
	FString DefaultPlayerName;
};
