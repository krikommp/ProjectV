// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCardInfo.h"

#include "GameplayAbilitySpec.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GridLocalPlayer.h"

UGridCardInfo::UGridCardInfo(): CardID(""), CardTypeID("") ,AbilityLevel(INDEX_NONE)
{
}

void UGridCardInfo::SaveCardInfoFlush()
{
	check(OwningPlayer);
	UGameplayStatics::SaveGameToSlot(this, GetSaveSlotName(), OwningPlayer->GetLocalPlayerIndex());
}

UGridCardInfo* UGridCardInfo::CreateOrLoadCardInfo(const UGridLocalPlayer* LocalPlayer, const FString& CardId, const FGridCardData& CardData)
{
	UGridCardInfo* CardInfo = nullptr;
	const FString SlotName = FString::Format(TEXT("GridCardInfo_{0}"), TArray<FStringFormatArg>({ CardId }));

	if (LocalPlayer != nullptr)
	{
		if (UGameplayStatics::DoesSaveGameExist(SlotName,LocalPlayer->GetLocalPlayerIndex()))
		{
			USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SlotName, LocalPlayer->GetLocalPlayerIndex());
			CardInfo = Cast<UGridCardInfo>(Slot);
		}
	}

	if (CardInfo == nullptr)
	{
		CardInfo = Cast<UGridCardInfo>(UGameplayStatics::CreateSaveGameObject(UGridCardInfo::StaticClass()));
		CardInfo->CardID = CardId;
		CardInfo->CardTypeID = CardData.CardID;
		CardInfo->AbilityLevel = CardData.DefaultCardAbilityLevel;
	}

	ensureAlwaysMsgf(CardInfo, TEXT("Can not create UGridCardInfo, crash!!"));

	CardInfo->CardData = CardData;
	CardInfo->Initialize(const_cast<UGridLocalPlayer*>(LocalPlayer));

	return CardInfo;
}

void UGridCardInfo::Initialize(UGridLocalPlayer* LocalPlayer)
{
	if (LocalPlayer != nullptr)
	{
		OwningPlayer = LocalPlayer;
	}
}

FString UGridCardInfo::GetSaveSlotName() const
{
	return FString::Format(TEXT("GridCardInfo_{0}"), TArray<FStringFormatArg>({ CardID }));
}

UGridGameplayAbility_Card* UGridCardInfo::GetCardAbilityInstance(UGridAbilitySystemComponent* InAbilitySystemComponent) const
{
	if (GrantedHandle.IsValid())
	{
		if (const FGameplayAbilitySpec* AbilitySpec = InAbilitySystemComponent->FindAbilitySpecFromHandle(GrantedHandle))
		{
			if (AbilitySpec->Ability && AbilitySpec->IsActive())
			{
				return Cast<UGridGameplayAbility_Card>(AbilitySpec->GetPrimaryInstance());
			}
		}
	}
	return nullptr;
}

bool UGridCardInfo::CheckAbilityCanActivate() const
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = GrantedHandle;
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

bool UGridCardInfo::TryActivateCardAbility()
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = GrantedHandle;
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

void UGridCardInfo::TryCancelCardAbility()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilityHandle(GrantedHandle);
	}
}

void UGridCardInfo::SendCardInputEvent(const FGridAbilityInputEvent& InputEvent)
{
	if (AbilitySystemComponent)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(GrantedHandle))
		{
			if (AbilitySpec->IsActive())
			{
				AbilitySystemComponent->OnGridAbilityInputEvent.Broadcast(GrantedHandle, InputEvent);
			}
		}
	}
}
