// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GridGameplayAbility.generated.h"

/**
 * UGridGameplayAbility
 *
 * 项目自定义 GameplayAbility
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class PROJECTVGAME_API UGridGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
};
