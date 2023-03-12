#include "GridApplyDamageExecution.h"

#include "AbilitySystem/Attributes/GridCombatSet.h"
#include "AbilitySystem/Attributes/GridHealthSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;

	FDamageStatics()
	{
		HealthDef = FGameplayEffectAttributeCaptureDefinition(UGridHealthSet::GetHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UGridCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		DefenseDef = FGameplayEffectAttributeCaptureDefinition(UGridHealthSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}


UGridApplyDamageExecution::UGridApplyDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
}

void UGridApplyDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                       FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float CurrentHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, EvaluateParameters, CurrentHealth);

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	float CurrentDefense = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluateParameters, CurrentDefense);

	// 进行伤害计算
	const float DamageDefenseDone = FMath::Clamp(BaseDamage, 0.0f, CurrentDefense);
	const float DamageHealthDone = FMath::Clamp(BaseDamage - DamageDefenseDone, 0.0f, CurrentHealth);

	// 应用伤害
	if (DamageDefenseDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGridHealthSet::GetDefenseAttribute(), EGameplayModOp::Additive, -DamageDefenseDone));
	}
	if (DamageHealthDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGridHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, -DamageHealthDone));
	}
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGridCombatSet::GetBaseDamageAttribute(), EGameplayModOp::Override, 0.0f));
}
