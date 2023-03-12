// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameplayTagContainer.h"
#include "GridTurnManagerComponent.generated.h"

class UGridExperienceDefinition;
class AGridChessPiece;
class AGridPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridOnSwitchChessPieceSelect);

USTRUCT(BlueprintType)
struct FActionQueue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag Faction;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<AGridChessPiece>> ChessPieces;
};

/**
 * UGridTurnManagerComponent
 *
 * 战斗回合管理器
 */
UCLASS()
class PROJECTVGAME_API UGridTurnManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGridTurnManagerComponent(const FObjectInitializer& ObjectInitializer);

	// ~UActorComponent interface
	virtual void OnRegister() override;
	// ~UActorComponent interface

	// 注册行动单位
	UFUNCTION(BlueprintCallable, Category="Grid|Turn")
	void RegisterActionUnit(const FGameplayTag& Faction, AGridChessPiece* ChessPiece);

	// 注销行动单位
	UFUNCTION(BlueprintCallable, Category="Grid|Turn")
	void UnRegisterActionUnit(const FGameplayTag& Faction, AGridChessPiece* ChessPiece);

	// 获取当前行动序列
	UFUNCTION(BlueprintCallable, Category="Grid|Turn", meta=(DisplayName = "GetCurrentActionQueue"))
	void K2_GetCurrentActionQueue(FActionQueue& OutActionQueue);
	FORCEINLINE const FActionQueue& GetCurrentActionQueue() const { return ActionQueues[GetCurrentActionIndex()]; }

	// 获取当前行动棋子，可能为空
	UFUNCTION(BlueprintPure, Category="Grid|Turn")
	AGridChessPiece* GetCurrentUnit() const;

	// 设置当前行动棋子
	UFUNCTION(BlueprintCallable, Category="Grid|Turn")
	bool SetCurrentUnit(AGridChessPiece* InChessPiece);

	// 判断该棋子是否位于当前行动序列中
	UFUNCTION(BlueprintCallable, Category="Grid|Turn")
	bool IsUnitInCurrentActionQueue(AGridChessPiece* InChessPiece);
	
	UFUNCTION(BlueprintCallable, Category="Grid|Turn")
	void StartTurn();

	// 鼠标点击棋子，可能返回空
	UFUNCTION(BlueprintCallable, Category="Grid|Turn")
	AGridChessPiece* GetChessPieceFormMouseClick(APlayerController* InPlayerController);
protected:
	FORCEINLINE int32 GetCurrentActionIndex() const { check(TurnCounter != INDEX_NONE); return TurnCounter % ActionQueues.Num(); }
	
private:
	// 回合计数器
	int32 TurnCounter;

	// 行动序列
	TArray<FActionQueue> ActionQueues;

	// 当前行动棋子
	TObjectPtr<AGridChessPiece> CurrentActionPiece;

public:
	// 回合是否结束
	UPROPERTY(BlueprintReadWrite, Category="Grid|Turn")
	bool bIsTurnOver = true;

	// 切换控制单位委托
	UPROPERTY(BlueprintAssignable, Category="Grid|Turn")
	FGridOnSwitchChessPieceSelect OnSwitchChessPieceSelect;

	FSimpleMulticastDelegate OnTurnStart;
	FSimpleMulticastDelegate OnTurnFinish;
};
