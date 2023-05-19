// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "GridGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** other gameplay effects that will be applied to the target of this effect if this effect applies */
	/** different from ConditionalGameplayEffects will judge target's gameplaytag */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridGameplayEffect)
	TArray<FConditionalGameplayEffect> ConditionalGameplayEffectsToTarget;
};
