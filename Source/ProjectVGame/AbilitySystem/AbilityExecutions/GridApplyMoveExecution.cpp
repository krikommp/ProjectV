// Fill out your copyright notice in the Description page of Project Settings.


#include "GridApplyMoveExecution.h"
#include "AbilitySystem/Attributes/GridActionSet.h"

struct FMoveStatics
{
	FGameplayEffectAttributeCaptureDefinition MoveRatioDef;
	FGameplayEffectAttributeCaptureDefinition MovePreStepCostDef;

	FMoveStatics()
	{
		MoveRatioDef = FGameplayEffectAttributeCaptureDefinition(UGridActionSet::GetMoveCostRatioAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		MovePreStepCostDef = FGameplayEffectAttributeCaptureDefinition(UGridActionSet::GetMovePreStepCostAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FMoveStatics& MoveStatics()
{
	static FMoveStatics Statics;
	return Statics;
}

UGridApplyMoveExecution::UGridApplyMoveExecution()
{
	RelevantAttributesToCapture.Add(MoveStatics().MoveRatioDef);
	RelevantAttributesToCapture.Add(MoveStatics().MovePreStepCostDef);
}

void UGridApplyMoveExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MoveRatio = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MoveStatics().MoveRatioDef, EvaluateParameters, MoveRatio);

	float MovePreStepCost = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MoveStatics().MovePreStepCostDef, EvaluateParameters, MovePreStepCost);

	const float FinalCost = FMath::Max(0.0f, MovePreStepCost * MoveRatio);
	if (FinalCost > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGridActionSet::GetActionPointAttribute(), EGameplayModOp::Additive, -FinalCost));
	}
}
