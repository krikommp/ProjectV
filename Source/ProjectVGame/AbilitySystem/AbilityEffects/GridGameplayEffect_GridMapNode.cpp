// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameplayEffect_GridMapNode.h"

bool FConductionGameplayEffect::CanApply(const FGameplayTagContainer& SourceTags, float SourceLevel) const
{
	return SourceTags.HasAll(RequiredSourceTags);
}

FGameplayEffectSpecHandle FConductionGameplayEffect::CreateSpec(FGameplayEffectContextHandle EffectContext,
	float SourceLevel) const
{
	const UGameplayEffect* EffectCDO = EffectClass ? EffectClass->GetDefaultObject<UGameplayEffect>() : nullptr;
	return EffectCDO ? FGameplayEffectSpecHandle(new FGameplayEffectSpec(EffectCDO, EffectContext, SourceLevel)) : FGameplayEffectSpecHandle();
}
