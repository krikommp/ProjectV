// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "UObject/Object.h"
#include "GridApplyMoveExecution.generated.h"

/**
 * UGridApplyMoveExecution
 *
 * 移动消耗
 */
UCLASS()
class PROJECTVGAME_API UGridApplyMoveExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UGridApplyMoveExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
