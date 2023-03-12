// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCombatSet.h"
#include "Net/UnrealNetwork.h"

UGridCombatSet::UGridCombatSet()
	: BaseDamage(0.0f),
	  BaseHealing(0.0f)
{
}

void UGridCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridCombatSet, BaseDamage, OldValue);
}

void UGridCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridCombatSet, BaseHealing, OldValue);
}

void UGridCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGridCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridCombatSet, BaseHealing, COND_OwnerOnly, REPNOTIFY_Always);
}