// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GridGameplayAbility.h"
#include "UObject/Object.h"
#include "SkillAsset.generated.h"

class USkillEffect;

/**
 * USkillAsset
 *
 * 基础的技能资源
 */
UCLASS(HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by base skill asset."))
class PROJECTVGAME_API USkillAsset : public UGridGameplayAbility
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category=Base)
	FText Name;
	UPROPERTY(BlueprintReadOnly, Category=Base)
	FText Description;
	UPROPERTY(BlueprintReadOnly, Category=Base)
	TObjectPtr<UTexture2D> Icon;

	// 应用 Recover 效果时需要激活的技能效果
	UPROPERTY(EditAnywhere, Category=Effect)
	TSubclassOf<USkillEffect> RecoverSkillEffect;

	// 应用 Damage 效果时需要激活的技能效果
	UPROPERTY(EditAnywhere, Category=Effect)
	TSubclassOf<USkillEffect> DamageSkillEffect;

	// Damage 计算类
	UPROPERTY(EditAnywhere, Category=Execution)
	TSubclassOf<UGameplayEffectExecutionCalculation> DamageExecution;
};
