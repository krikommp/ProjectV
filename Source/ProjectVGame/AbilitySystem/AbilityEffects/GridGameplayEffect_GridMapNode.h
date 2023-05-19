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

	/** gameplay effect that will be applied to the target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffect)
	TSubclassOf<UGameplayEffect> EffectClass;

	/** Tags that the source must have for this GE to apply */
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridGameplayEffect, meta = (DisplayName = "AttachGameplayEffects"))
	TArray<TSubclassOf<UGameplayEffect>> AttachGameplayEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridGameplayEffect)
	TArray<FConductionGameplayEffect> ConductionGameplayEffects;
};
