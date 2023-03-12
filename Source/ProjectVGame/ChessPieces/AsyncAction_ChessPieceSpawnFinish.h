// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ChessPieceSpawnFinish.generated.h"

class UWorld;
class AGameStateBase;
class AGridChessPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChessPieceSpawnFinishDelegate);

/**
 * UAsyncAction_ChessPieceSpawnFinish
 *
 * 异步等待棋子放置完毕
 */
UCLASS()
class PROJECTVGAME_API UAsyncAction_ChessPieceSpawnFinish : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ChessPieceSpawnFinish* WaitForChessPieceSpawnFinish(UObject* WorldContextObject);

	virtual void Activate() override;

public:

	UPROPERTY(BlueprintAssignable)
	FChessPieceSpawnFinishDelegate OnFinish;

private:

	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToChessPieceSpawn(AGameStateBase* GameState);
	void Step3_HandleSpawnFinished(const TArray<AGridChessPiece*>& InChessPieceArray);
	void Step4_BroadcastReady();

	TWeakObjectPtr<UWorld> WorldPtr;
};
