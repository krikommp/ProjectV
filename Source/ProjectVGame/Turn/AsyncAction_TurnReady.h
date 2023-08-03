// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_TurnReady.generated.h"

/**
 * UAsyncAction_TurnReady
 *
 * 等待关卡准备完毕
 * 代表着关卡内逻辑可以开始执行
 */
UCLASS()
class PROJECTVGAME_API UAsyncAction_TurnReady : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_TurnReady* WaitForTurnReady(UObject* WorldContextObject);
	virtual void Activate() override;
	
private:
	TWeakObjectPtr<UWorld> WorldPtr;

public:
	UPROPERTY(BlueprintAssignable)
	FSimpleMulticastDelegate OnReady;

private:
	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToTurnReady(AGameStateBase* GameState);
	void Step3_HandleTurnReady();
	void Step4_BroadcastReady();
};
