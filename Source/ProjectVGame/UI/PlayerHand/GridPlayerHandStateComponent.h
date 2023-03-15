// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputModeTypes.h"
#include "Components/GameStateComponent.h"
#include "GameModes/GridExperienceDefinition.h"
#include "UObject/Object.h"
#include "GridPlayerHandStateComponent.generated.h"

class UGridPlayerHand;
class UGridExperienceDefinition;
class UCommonActivatableWidget;

/**
 * UGridPlayerHandStateComponent
 *
 * 手牌UI组件
 */
UCLASS(Config=Game)
class PROJECTVGAME_API UGridPlayerHandStateComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGridPlayerHandStateComponent(const FObjectInitializer& ObjectInitializer);

	// 获取手牌窗口
	UFUNCTION(BlueprintPure, Category="Grid|PlayerHand")
	FORCEINLINE UGridPlayerHand* GetPlayerHand() const { return PlayerHand; }

	// 判断当前手牌中的所有卡牌是否运动完成
	UFUNCTION(BlueprintPure, Category="Grid|PlayerHand")
	bool CheckAllCardInHandMotionEnded();

	// 重置手牌状态
	UFUNCTION(BlueprintCallable, Category="Grid|PLayerHand")
	void ResetPlayerHand();

	// 显示手牌界面
	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void ShowPlayerHand();
	
	// 隐藏手牌界面
	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void HidePlayerHand();

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void StartToCheckMotion() { bStartToCheckMotion = true; }

protected:
	virtual void OnRegister() override;
	void OnExperienceLoaded(const UGridExperienceDefinition* Experience);
	void ActivatePlayerHand();
	
protected:

	UPROPERTY(Transient)
	TObjectPtr<UGridPlayerHand> PlayerHand;

	UPROPERTY(Config)
	TSoftClassPtr<UCommonActivatableWidget> PlayerHandClass;

	bool bStartToCheckMotion;
};
