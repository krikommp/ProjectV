// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "UObject/Object.h"
#include "GridApplyCostExecution.generated.h"

/**
 * UGridApplyCostExecution
 *
 * 应用技能消耗
 */
UCLASS()
class PROJECTVGAME_API UGridApplyCostExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:

	UGridApplyCostExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
