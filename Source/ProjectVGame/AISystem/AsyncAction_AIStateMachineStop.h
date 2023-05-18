// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "AsyncAction_AIStateMachineStop.generated.h"

class AGridChessPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncChessPieceAIStateMachineStopped);

/**
 * UAsyncAction_AIStateMachineStop
 *
 * 异步等待AI状态机停止
 */
UCLASS()
class PROJECTVGAME_API UAsyncAction_AIStateMachineStop : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_AIStateMachineStop* WaitForAIStateMachineStopped(UObject* WorldContextObject, AGridChessPiece* InTarget);

	virtual void Activate() override;
	
	UPROPERTY(BlueprintAssignable, Category="Grid|Async Action")
	FOnAsyncChessPieceAIStateMachineStopped OnAsyncChessPieceAIStateMachineStopped;

private:

	void HandleAIStateMachineStopped();

	TWeakObjectPtr<AGridChessPiece> Target;
	TWeakObjectPtr<UWorld> WorldPtr;
};
