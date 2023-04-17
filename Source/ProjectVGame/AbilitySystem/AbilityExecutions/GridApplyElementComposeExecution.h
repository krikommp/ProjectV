// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "UObject/Object.h"
#include "GridApplyElementComposeExecution.generated.h"

/**
 * UGridApplyElementComposeExecution
 *
 * 元素组合应用
 */
UCLASS()
class PROJECTVGAME_API UGridApplyElementComposeExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGridApplyElementComposeExecution();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FConditionalGameplayEffect> ConditionalGameplayEffects;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
