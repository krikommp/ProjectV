// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSkillInfo.h"

#include "GameplayAbilitySpec.h"
#include "GridCardInfo.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GridLocalPlayer.h"

UGridSkillInfo::UGridSkillInfo()
{
}

UGridSkillInfo* UGridSkillInfo::CreateOrLoadSkillInfo(const UGridLocalPlayer* InLocalPlayer,
                                                      int32 HeroId, const FGridSkillData& InSkillData)
{
	UGridSkillInfo* SkillInfo = nullptr;
	const FString SlotName = FString::Format(TEXT("GridSkillInfo_{0}_{1}"), TArray<FStringFormatArg>({ HeroId, InSkillData.ID.ToString() }));

	if (InLocalPlayer != nullptr)
	{
		if (UGameplayStatics::DoesSaveGameExist(SlotName,InLocalPlayer->GetLocalPlayerIndex()))
		{
			USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SlotName, InLocalPlayer->GetLocalPlayerIndex());
			SkillInfo = Cast<UGridSkillInfo>(Slot);
		}
	}

	if (SkillInfo == nullptr)
	{
		SkillInfo = Cast<UGridSkillInfo>(UGameplayStatics::CreateSaveGameObject(UGridSkillInfo::StaticClass()));
		SkillInfo->ID = InSkillData.ID;
		SkillInfo->HeroId = HeroId;
		SkillInfo->Level = InSkillData.Level;
	}

	ensureAlwaysMsgf(SkillInfo, TEXT("Can not create UGridSkillInfo, crash!!"));

	SkillInfo->SkillData = InSkillData;

	return SkillInfo;
}

FString UGridSkillInfo::GetSaveSlotName() const
{
	return FString::Format(TEXT("GridSkillInfo_{0}_{1}"), TArray<FStringFormatArg>({ HeroId, ID.ToString() }));
}

bool UGridSkillInfo::CheckAbilityCanActivate() const
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = AbilitySpecHandle;
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				return AbilitySpec->Ability->CanActivateAbility(SpecHandle, AbilitySystemComponent->AbilityActorInfo.Get());
			}
		}
	}
	return false;
}

UGridGameplayAbility_Card* UGridSkillInfo::GetAbility() const
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = AbilitySpecHandle;
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle))
		{
			return Cast<UGridGameplayAbility_Card>(AbilitySpec->Ability);
		}
	}
	return nullptr;
}

bool UGridSkillInfo::TryActivateSkill()
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = AbilitySpecHandle;
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				return AbilitySystemComponent->TryActivateAbility(SpecHandle);
			}
		}
	}
	return false;
}

void UGridSkillInfo::SendInputEvent(const FGridAbilityInputEvent& InputEvent)
{
	if (AbilitySystemComponent)
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			if (AbilitySpec->IsActive())
			{
				AbilitySystemComponent->OnGridAbilityInputEvent.Broadcast(AbilitySpecHandle, InputEvent);
			}
		}
	}
}
