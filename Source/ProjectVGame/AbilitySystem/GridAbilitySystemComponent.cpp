// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#ifdef ENABLE_TURN_BASE_GAME_ABILITY
#include "AbilityTimerManager.h"
#endif
#include "GridGlobalAbilitySystem.h"
#include "GridLogChannel.h"
#include "Abilities/GridGameplayAbility_Card.h"
#include "AbilityEffects/GridGameplayEffect.h"
#include "AbilityEffects/GridGameplayEffect_GridMapNode.h"
#include "GridMapManager/GridMapNode.h"
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

FActiveGameplayEffectHandle UGridAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(
	const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey)
{
	const auto MyHandle = Super::ApplyGameplayEffectSpecToSelf(GameplayEffect, PredictionKey);
	if (const UGridGameplayEffect* GridGameplayEffect = Cast<UGridGameplayEffect>(GameplayEffect.Def))
	{
		// 判断目标身上是否有需要的GameplayTag, 如果有则应用GameplayEffect
		for (const FConditionalGameplayEffect& ConditionalGameplayEffect : GridGameplayEffect->ConditionalGameplayEffectsToTarget)
		{
			if (GridGameplayEffect->StaticClass() == ConditionalGameplayEffect.EffectClass)
			{
				UE_LOG(LogGridAbilitySystem, Error, TEXT("不能在自己的ConditionalGameplayEffectsToTarget中添加自己的类"));
				continue;
			}
			if (HasAllMatchingGameplayTags(ConditionalGameplayEffect.RequiredSourceTags))
			{
				FGameplayEffectSpecHandle ConditionalSpecHandle = ConditionalGameplayEffect.CreateSpec(GameplayEffect.GetEffectContext(), GameplayEffect.GetLevel());
				if (ConditionalSpecHandle.IsValid())
				{
					ApplyGameplayEffectSpecToSelf(*ConditionalSpecHandle.Data.Get(), PredictionKey);
				}
			}
		}
	}
	// 该GameplayEffect是否需要应用到相邻的格子上
	if (const UGridGameplayEffect_GridMapNode* GridMapNodeGameplayEffect = Cast<UGridGameplayEffect_GridMapNode>(GameplayEffect.Def))
	{
		if (const AGridMapNode* GridMapNode =  Cast<AGridMapNode>(GetOwnerActor()))
		{
			TArray<const AGridMapNode*> NearbyNodes;
			for(const FConductionGameplayEffect& ConductionGameplayEffect : GridMapNodeGameplayEffect->ConductionGameplayEffects)
			{
				GridMapNode->FindAllNearbyTiles(NearbyNodes, ConductionGameplayEffect.RequiredSourceTags);
				FGameplayEffectSpecHandle ConductionSpecHandle = ConductionGameplayEffect.CreateSpec(GameplayEffect.GetContext(), GameplayEffect.GetLevel());
				if (ConductionSpecHandle.IsValid())
				{
					for (const AGridMapNode* ApplyToGridMapNode : NearbyNodes)
					{
						ApplyToGridMapNode->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*ConductionSpecHandle.Data.Get(), PredictionKey);
					}
				}
			}
		}
	}
	return MyHandle;
}

void UGridAbilitySystemComponent::TickAbilityTurn(int32 Delta)
{
#ifdef ENABLE_TURN_BASE_GAME_ABILITY
	const auto TimerManager = UAbilitySystemGlobals::Get().GetTimerManager();
	if (TimerManager == nullptr)
	{
		return;
	}
	TimerManager->TickTurn(this, Delta);
#endif
}

TMap<UGridAbilityBuffUIData*, float> UGridAbilitySystemComponent::GetAllActiveBuffInfos()
{
	TMap<UGridAbilityBuffUIData*, float> Out;
	const FActiveGameplayEffectsContainer& ActiveGameEffectContainer = GetActiveGameplayEffects();
	for (FActiveGameplayEffectsContainer::ConstIterator ActiveEffectIt = ActiveGameEffectContainer.CreateConstIterator(); ActiveEffectIt; ++ActiveEffectIt)
	{
		const UGameplayEffect* Effect = ActiveEffectIt->Spec.Def;
		if (const UGridAbilityBuffUIData* UIData = Cast<UGridAbilityBuffUIData>(UAbilitySystemBlueprintLibrary::GetGameplayEffectUIData(Effect->GetClass(), UGridAbilityBuffUIData::StaticClass())))
		{
			Out.Add(const_cast<UGridAbilityBuffUIData*>(UIData), ActiveEffectIt->GetDuration());
		}
	}
	return Out;
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
