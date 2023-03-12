// Fill out your copyright notice in the Description page of Project Settings.


#include "GridHealthSet.h"

#include "GameplayEffectExtension.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/GridVerbMessage.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Grid_Damge_Message, "Grid.Damage.Message");
UE_DEFINE_GAMEPLAY_TAG(TAG_Grid_Defense_Message, "Grid.Defense.Message");

UGridHealthSet::UGridHealthSet()
	: Health(100.0f),
	  MaxHealth(100.f),
	  Defense(0.0f)
{
	bOutOfHealth = false;
}

void UGridHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGridHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGridHealthSet, Defense, COND_None, REPNOTIFY_Always);
}

void UGridHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridHealthSet, Health, OldValue);
}

void UGridHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridHealthSet, MaxHealth, OldValue);
}

void UGridHealthSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGridHealthSet, Defense, OldValue);
}

bool UGridHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	return true;
}

void UGridHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 减少血量
		if (Data.EvaluatedData.Magnitude <= 0.0f)
		{
			// 向其他观察系统发送一个verb消息
			FGridVerbMessage Message;
			Message.Verb = TAG_Grid_Damge_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		if (GetHealth() <= 0.0f && !bOutOfHealth)
		{
			if (OnOutOfHealth.IsBound())
			{
				const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();

				OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
			}
		}

		bOutOfHealth = (GetHealth() <= 0.f);
	}else if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
	{
		// 护盾值减少
		if (Data.EvaluatedData.Magnitude <= 0.0f)
		{
			// 向其他观察系统发送一个verb消息
			FGridVerbMessage Message;
			Message.Verb = TAG_Grid_Defense_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		if (GetDefense() <= 0.0f)
		{
			if (OnOutOfDefense.IsBound())
			{
				const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();

				OnOutOfDefense.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
			}
		}
	}else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(GetHealth() + GetHealing());
		SetHealing(0.0f);
	}
}

void UGridHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UGridHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UGridHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// 确保生命值不会高于最大生命值
		if (GetHealth() > NewValue)
		{
			UGridAbilitySystemComponent* GridASC = GetGridAbilitySystemComponent();
			check(GridASC);

			GridASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && GetHealth() > 0.0f)
	{
		bOutOfHealth = false;
	}
}

void UGridHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// 不允许生命值高于最高生命值
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}else if (Attribute == GetMaxHealthAttribute())
	{
		// 不允许最高生命值低于1.0f
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}