// Fill out your copyright notice in the Description page of Project Settings.


#include "GridApplyElementComposeExecution.h"

#include "AbilitySystemComponent.h"

UGridApplyElementComposeExecution::UGridApplyElementComposeExecution()
{
}

void UGridApplyElementComposeExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TArray<FGameplayEffectSpecHandle> TargetEffectSpecs;
	for (const FConditionalGameplayEffect& ConditionalEffect : ConditionalGameplayEffects)
	{
		if(ExecutionParams.GetTargetAbilitySystemComponent()->HasAllMatchingGameplayTags(ConditionalEffect.RequiredSourceTags))
		{
			FGameplayEffectSpecHandle SpecHandle = ExecutionParams.GetSourceAbilitySystemComponent()->MakeOutgoingSpec(ConditionalEffect.EffectClass, ExecutionParams.GetOwningSpec().GetLevel(), ExecutionParams.GetOwningSpec().GetContext());
			if (SpecHandle.IsValid())
			{
				TargetEffectSpecs.Add(SpecHandle);
			}
		}
	}
	for(const FGameplayEffectSpecHandle& ApplyEffect : TargetEffectSpecs)
	{
		ExecutionParams.GetTargetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*ApplyEffect.Data.Get());
	}
}
