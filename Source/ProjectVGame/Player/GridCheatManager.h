// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Logging/LogMacros.h"
#include "GridCheatManager.generated.h"

#ifndef USING_CHEAT_MANAGER
#define USING_CHEAT_MANAGER (1 && !UE_BUILD_SHIPPING)
#endif // #ifndef USING_CHEAT_MANAGER

DECLARE_LOG_CATEGORY_EXTERN(LogGridCheat, Log, All);

class AGridPlayerController;

/**
 * UGridCheatManager
 *
 * 项目自定义作弊器
 */
UCLASS(config = Game, Within = PlayerController, MinimalAPI)
class UGridCheatManager : public UCheatManager
{
	GENERATED_BODY()

	virtual void InitCheatManager() override;

	// 向命令行输出Log信息
	static void CheatOutputText(const FString& TextToOutput);

	UFUNCTION(Exec)
	void AddNewCard(const FString& InCardID);

protected:

	TObjectPtr<AGridPlayerController> LocalPlayerController;
};
