// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"

#include "ChessTurnComponent.generated.h"

/**
 * UChessTurnComponent
 *
 * 用于管理战斗场景中的回合
 */
UCLASS()
class PROJECTVGAME_API UChessTurnComponent : public UGameStateComponent
{
	GENERATED_UCLASS_BODY()

	//~ Begin UActorComponent Interface.
	virtual void OnRegister() override;
	//~ Begin UActorComponent Interface.

	void CallOrRegister_OnTurnReady(FSimpleMulticastDelegate::FDelegate&& Delegate);

	bool IsReady() const { return bReadyToStartTurn; }

private:
	void OnTilemapLoaded();

public:
	// 开始一个回合
	UFUNCTION(BlueprintCallable, Category=Turn)
	void StartTurn();
	// 结束一个回合
	UFUNCTION(BlueprintCallable, Category=Turn)
	void SwitchTurn();

	// 判断当前是否为玩家回合
	UFUNCTION(BlueprintPure, Category=Turn)
	bool CheckIsPlayerTurn() const;

private:
	// 是否可以开始第一轮回合
	uint8 bReadyToStartTurn : 1;
	// 回合计数器
	int32 TurnCount;

	FSimpleMulticastDelegate OnReady;
};
