// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActionSet.h"

#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UGridActionSet::UGridActionSet()
	: ActionPoint(3.0f),
	  MaxActionPoint(3.0f),
	  MoveCostRatio(1.0f),
	  MovePreStepCost(1.0f),
	  EveryTurnDrawCardsNum(3.0f),
	  VisibleRange(5.0f)
{
}

void UGridActionSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGridActionSet, ActionPoint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridActionSet, MaxActionPoint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridActionSet, MoveCostRatio, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridActionSet, MovePreStepCost, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridActionSet, VisibleRange, COND_None, REPNOTIFY_Always);
}

void UGridActionSet::OnRep_ActionPoint(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridActionSet, ActionPoint, OldValue);
}

void UGridActionSet::OnRep_MaxActionPoint(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridActionSet, MaxActionPoint, OldValue);
}

void UGridActionSet::OnReq_MoveCostRatio(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridActionSet, MoveCostRatio, OldValue);
}

void UGridActionSet::OnReq_MovePreStepCost(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridActionSet, MovePreStepCost, OldValue);
}

void UGridActionSet::OnReq_EveryTurnDrawCardsNum(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridActionSet, EveryTurnDrawCardsNum, OldValue);
}

void UGridActionSet::OnReq_VisibleRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridActionSet, VisibleRange, OldValue);
}

void UGridActionSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UGridActionSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UGridActionSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxActionPointAttribute())
	{
		if (GetActionPoint() > NewValue)
		{
			UGridAbilitySystemComponent* GridASC = GetGridAbilitySystemComponent();
			check(GridASC);

			GridASC->ApplyModToAttribute(GetActionPointAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UGridActionSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetActionPointAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxActionPoint());
	}
	else if (Attribute == GetMaxActionPointAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetMoveCostRatioAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}