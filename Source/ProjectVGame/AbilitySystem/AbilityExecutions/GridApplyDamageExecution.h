

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GridApplyDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridApplyDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGridApplyDamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
