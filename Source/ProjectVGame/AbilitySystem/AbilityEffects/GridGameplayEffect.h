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

	// 根据目标身上的Tag来决定是否应用下面的GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GridGameplayEffect)
	TArray<FConditionalGameplayEffect> ConditionalGameplayEffectsToTarget;
};
