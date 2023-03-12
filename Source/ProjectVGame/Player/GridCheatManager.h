// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Logging/LogMacros.h"
#include "GridCheatManager.generated.h"

/**
 * UGridCheatManager
 *
 * 项目自定义作弊器
 */
UCLASS(config = Game, Within = PlayerController, MinimalAPI)
class UGridCheatManager : public UCheatManager
{
	GENERATED_BODY()
};
