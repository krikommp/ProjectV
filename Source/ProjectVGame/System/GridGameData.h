// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GridGameData.generated.h"

class UGameplayEffect;

/**
 * UGridGameData
 *
 * 用于保存全局游戏资源的 DataAsset
 */
UCLASS(BlueprintType, Const, meta=(DisplayName = "Grid Game Data", ShortTooltip  = "包含全局游戏资源."))
class PROJECTVGAME_API UGridGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGridGameData();

	static const UGridGameData& Get();

public:

	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	UPROPERTY(EditDefaultsOnly, Category="Default Gameplay Effect", meta=(DisplayName = "Move GamePlay Effect (SetByCaller)", ShortTooltip="移动消耗的 GE"))
	TSoftObjectPtr<UGameplayEffect> MoveCostGameplayEffect_SetByCaller;
};
