// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMoveSet.h"

#include "GameplayEffectExtension.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

UGridMoveSet::UGridMoveSet()
	: Acceleration(20.0f),
	  MaxSpeed(600.0f)
{
}

void UGridMoveSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGridMoveSet, Acceleration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridMoveSet, MaxSpeed, COND_None, REPNOTIFY_Always);
}

void UGridMoveSet::OnRep_Acceleration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridMoveSet, Acceleration, OldValue);
}

void UGridMoveSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridMoveSet, MaxSpeed, OldValue);
}
