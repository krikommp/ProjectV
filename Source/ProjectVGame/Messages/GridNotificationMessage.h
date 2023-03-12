// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GridNotificationMessage.generated.h"

class APlayerState;

/**
 * FGridNotificationMessage
 *
 * 发出临时日志的消息
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridNotificationMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category=Notification)
	FGameplayTag TargetChannel;

	// 目标玩家，如果为空，代表向所有玩家发送消息
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	TObjectPtr<APlayerState> TargetPlayer = nullptr;

	// 显示的消息
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	FText PayloadMessage;

	// 额外的 GameplayTag
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	FGameplayTag PayloadTag;

	// 额外的对象
	UPROPERTY(BlueprintReadWrite, Category=Notification)
	TObjectPtr<UObject> PayloadObject = nullptr;
};
