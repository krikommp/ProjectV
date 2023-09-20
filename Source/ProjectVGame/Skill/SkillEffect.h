// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityEffects/GridGameplayEffect.h"
#include "UObject/Object.h"
#include "SkillEffect.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API USkillEffect : public UGridGameplayEffect
{
	GENERATED_UCLASS_BODY()

public:
	// 技能效果名字
	UPROPERTY(BlueprintReadOnly, Category=Base)
	FName Name;

	// 技能效果是否被激活
	UPROPERTY()
	bool bActive;
};
