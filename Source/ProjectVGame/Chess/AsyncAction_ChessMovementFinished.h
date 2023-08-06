// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "AsyncAction_ChessMovementFinished.generated.h"

class AGridChessBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChessMovementFinishedAsyncDelegate);

/**
 * UAsyncAction_ChessMovementFinished
 *
 * 异步等待棋子移动完毕
 */
UCLASS()
class PROJECTVGAME_API UAsyncAction_ChessMovementFinished : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ChessMovementFinished* WaitForChessMovementFinished(UObject* WorldContextObject, AGridChessBase* InChess);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FChessMovementFinishedAsyncDelegate OnFinished;

private:
	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToChessMovementFinished(AGameStateBase* GameState);
	void Step3_HandleChessMovementFinished();
	void Step4_BroadcastFinished();

	TWeakObjectPtr<UWorld> WorldPtr;
	TWeakObjectPtr<AGridChessBase> Chess;
};
