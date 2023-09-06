// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GridGameplayAbility.h"
#include "UObject/Object.h"
#include "SkillBaseAsset.generated.h"

/**
 * USkillBaseAsset
 *
 * 基础的技能资源
 */
UCLASS(HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by base skill asset."))
class PROJECTVGAME_API USkillBaseAsset : public UGridGameplayAbility
{
	GENERATED_BODY()
};
