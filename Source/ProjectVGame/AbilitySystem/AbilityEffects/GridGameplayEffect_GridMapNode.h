// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GridGameplayEffect.h"
#include "UObject/Object.h"
#include "GridGameplayEffect_GridMapNode.generated.h"

USTRUCT(BlueprintType)
struct PROJECTVGAME_API FConductionGameplayEffect
{
	GENERATED_USTRUCT_BODY()

	bool CanApply(const FGameplayTagContainer& SourceTags, float SourceLevel) const;

	FGameplayEffectSpecHandle CreateSpec(FGameplayEffectContextHandle EffectContext, float SourceLevel) const;

	// 定义会被应用到目标的GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffect)
	TSubclassOf<UGameplayEffect> EffectClass;

	// 定义目标需要的Tag,如果目标身上没有这些Tag,那么GE将不会应用到目标身上
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffect)
	FGameplayTagContainer RequiredSourceTags;
};

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridGameplayEffect_GridMapNode : public UGridGameplayEffect
{
	GENERATED_BODY()

public:
	// 针对GridMapNode使用, 可以指定一系列GE，这些GE将会对进入该格子的棋子施加
	// 并且每回合都会执行一次
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridGameplayEffect)
	TArray<TSubclassOf<UGameplayEffect>> AttachGameplayEffects;

	// 针对GridMapNode使用，会将对应的GE施加到这个单位格邻近的格子上（包括他自己）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridGameplayEffect)
	TArray<FConductionGameplayEffect> ConductionGameplayEffects;
};
