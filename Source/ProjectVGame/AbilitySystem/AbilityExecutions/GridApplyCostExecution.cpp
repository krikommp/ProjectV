// Fill out your copyright notice in the Description page of Project Settings.


#include "GridApplyCostExecution.h"

#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"
#include "AbilitySystem/Attributes/GridActionSet.h"

struct FGridActionPointStatics
{
	FGameplayEffectAttributeCaptureDefinition ActionPointsDef;

	FGridActionPointStatics()
	{
		ActionPointsDef = FGameplayEffectAttributeCaptureDefinition(UGridActionSet::GetActionPointAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static const FGridActionPointStatics& ActionPointStatics()
{
	static FGridActionPointStatics ActionPointStatics;
	return ActionPointStatics;
}

UGridApplyCostExecution::UGridApplyCostExecution()
{
	RelevantAttributesToCapture.Add(ActionPointStatics().ActionPointsDef);
}

void UGridApplyCostExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float ActionPoints = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ActionPointStatics().ActionPointsDef, EvaluateParameters, ActionPoints);

	const UGridGameplayAbility_Card* Ability = Cast<UGridGameplayAbility_Card>(Spec.GetContext().GetAbilityInstance_NotReplicated());
	const float AbilityCost = Ability->GetCardAbilityCost(Ability->GetAbilityLevel());

	const float FinalActionPoint = FMath::Max(ActionPoints - AbilityCost, 0.0f);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGridActionSet::GetActionPointAttribute(), EGameplayModOp::Override, FinalActionPoint));
}
