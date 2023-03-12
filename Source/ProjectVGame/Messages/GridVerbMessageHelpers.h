// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridVerbMessage.h"
#include "AbilitySystem/GridGameplayCueManager.h"
#include "GridVerbMessageHelpers.generated.h"

/**
 * UGridVerbMessageHelpers
 *
 * 动作消息辅助函数
 */
UCLASS()
class PROJECTVGAME_API UGridVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Grid")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category="Grid")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category="Grid")
	static FGameplayCueParameters VerbMessageToCueParameters(const FGridVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category="Grid")
	static FGridVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
