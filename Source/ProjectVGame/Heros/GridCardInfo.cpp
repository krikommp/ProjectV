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

bool UGridCardInfo::CheckAbilityCanActivate(FGameplayTagContainer& OptionalRelevantTags) const
{
	if (AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = GrantedHandle;
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				return AbilitySpec->Ability->CanActivateAbility(SpecHandle, AbilitySystemComponent->AbilityActorInfo.Get(), nullptr, nullptr, &OptionalRelevantTags);
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

FText UGridCardInfo::GetCardDescription()
{
	check(AbilitySystemComponent);

	if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(GrantedHandle))
	{
		if (UGridGameplayAbility_Card* CardAbility = Cast<UGridGameplayAbility_Card>(AbilitySpec->Ability))
		{
			if (const FGridGameplayEffectContainer* FoundContainer = CardAbility->EffectContainers.Find(CardData.ContainerTag))
			{
				FFormatOrderedArguments Args;
				int32 ArgIndex = 0;
				for (const TSubclassOf<UGameplayEffect>& EffectClass : FoundContainer->TargetGameplayEffects)
				{
					if (ArgIndex >= CardData.AbilityEffectArgs.Num()) break;
					FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, AbilityLevel, CardAbility->MakeEffectContext(GrantedHandle, AbilitySystemComponent->AbilityActorInfo.Get()));
					FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
					FGameplayTagContainer TagContainer;
					Spec->GetAllAssetTags(TagContainer);
					if (TagContainer.HasTagExact(CardData.AbilityEffectArgs[ArgIndex].GameplayTag))
					{
						float Result = 0.0f;
						Spec->CalculateModifierMagnitudes();
						for (int32 ModIdx = 0; ModIdx < Spec->Modifiers.Num();  ++ModIdx)
						{
							if (Spec->Def->Modifiers[ModIdx].Attribute == CardData.AbilityEffectArgs[ArgIndex].Attribute)
							{
								Result = Spec->Modifiers[ModIdx].GetEvaluatedMagnitude();
								break;
							}
						}
						Args.Add(Result);
						++ArgIndex;
					}
				}
				return FText::Format(CardData.CardDescription, Args);
			}
		}
	}

	return FText::GetEmpty();
}
