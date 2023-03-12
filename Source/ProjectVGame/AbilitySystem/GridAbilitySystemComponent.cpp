// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilityTimerManager.h"
#include "GridGlobalAbilitySystem.h"
#include "Abilities/GridGameplayAbility_Card.h"
#include "UIData/GridAbilityBuffUIData.h"

UGridAbilitySystemComponent::UGridAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGridAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGridGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UGridGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void UGridAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if(bHasNewPawnAvatar)
	{
		if (UGridGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UGridGlobalAbilitySystem>(GetWorld()))
		{
			GlobalAbilitySystem->RegisterASC(this);
		}

		OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::NotifyGameplayEffectActivate);
		OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &ThisClass::NotifyGameplayEffectRemoved);
	}
}

void UGridAbilitySystemComponent::TickAbilityTurn(int32 Delta)
{
	const auto TimerManager = UAbilitySystemGlobals::Get().GetTimerManager();
	if (TimerManager == nullptr)
	{
		return;
	}
	TimerManager->TickTurn(this, Delta);
}

void UGridAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
                                                     bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	if (UGridGameplayAbility_Card* CardAbility = Cast<UGridGameplayAbility_Card>(Ability))
	{
		OnCardAbilityEnded.Broadcast(CardAbility, bWasCancelled);
	}
}

void UGridAbilitySystemComponent::OnGameplayEffectDurationChange(FActiveGameplayEffect& ActiveEffect)
{
	NotifyGameplayEffectRefresh(ActiveEffect);
}

void UGridAbilitySystemComponent::NotifyGameplayEffectActivate(UAbilitySystemComponent* InASC, const FGameplayEffectSpec& InEffectSpec,
	FActiveGameplayEffectHandle InActiveEffectHandle)
{
	if (const UGameplayEffect* Effect = InEffectSpec.Def)
	{
		if (const UGridAbilityBuffUIData* UIData = Cast<UGridAbilityBuffUIData>(UAbilitySystemBlueprintLibrary::GetGameplayEffectUIData(Effect->GetClass(), UGridAbilityBuffUIData::StaticClass())))
		{
			OnAbilityBuffActive.Broadcast(UIData, InEffectSpec.GetDuration());
		}
	}
}

void UGridAbilitySystemComponent::NotifyGameplayEffectRemoved(const FActiveGameplayEffect& InActiveEffect)
{
	if (const UGameplayEffect* Effect = InActiveEffect.Spec.Def)
	{
		if (const UGridAbilityBuffUIData* UIData = Cast<UGridAbilityBuffUIData>(UAbilitySystemBlueprintLibrary::GetGameplayEffectUIData(Effect->GetClass(), UGridAbilityBuffUIData::StaticClass())))
		{
			OnAbilityBuffRemoved.Broadcast(UIData);
		}
	}
}

void UGridAbilitySystemComponent::NotifyGameplayEffectRefresh(const FActiveGameplayEffect& InActiveEffect)
{
	if (const UGameplayEffect* Effect = InActiveEffect.Spec.Def)
	{
		if (const UGridAbilityBuffUIData* UIData = Cast<UGridAbilityBuffUIData>(UAbilitySystemBlueprintLibrary::GetGameplayEffectUIData(Effect->GetClass(), UGridAbilityBuffUIData::StaticClass())))
		{
			OnAbilityBuffRefresh.Broadcast(UIData, InActiveEffect.Spec.GetDuration());
		}
	}
}
