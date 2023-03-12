// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceSkillComponent.h"

#include "GridChessPiece.h"
#include "GridChessPieceComponent.h"
#include "GridLogChannel.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

#include "GridChessPieceExtensionComponent.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"
#include "Heros/GridHeroInfo.h"
#include "Heros/GridSkillInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridChessPieceSkillComponent)

UGridChessPieceSkillComponent::UGridChessPieceSkillComponent(const FObjectInitializer& ObjectInitialize)
	: Super(ObjectInitialize)
{
}

UGridSkillInfo* UGridChessPieceSkillComponent::FindSkillInfoByAbility(
	TSubclassOf<UGridGameplayAbility_Card> AbilityClass) const
{
	if (!AbilitySystemComponent || !HeroInfo.IsValid()) return nullptr;
	
	const auto AbilitySpecHandle = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);

	if (AbilitySpecHandle == nullptr) return nullptr;

	for (const auto SkillPair : HeroInfo->SkillInfoMap)
	{
		UGridSkillInfo* SkillInfo = SkillPair.Value;
		if (SkillInfo->AbilitySpecHandle == AbilitySpecHandle->Handle)
		{
			return SkillInfo;
		}
	}

	return nullptr;
}

void UGridChessPieceSkillComponent::OnRegister()
{
	Super::OnRegister();

	if (const AGridChessPiece* Pawn = GetPawn<AGridChessPiece>())
	{
		if (UGridChessPieceComponent* ChessPieceComponent = UGridChessPieceComponent::FindGridChessPieceComponent(Pawn))
		{
			ChessPieceComponent->CallOrRegister_ChessPieceInitialized(
				FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnChessPieceInitialized));
		}
	}
	else
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[UGridChessPieceSkillComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("UGridChessPieceSkillComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName ChessPieceSkillLogName = TEXT("GridChessPieceSkillComponent");

			FMessageLog(ChessPieceSkillLogName).Error()
			                                   ->AddToken(FUObjectToken::Create(
				                                   this, FText::FromString(GetNameSafe(this))))
			                                   ->AddToken(FTextToken::Create(Message));

			FMessageLog(ChessPieceSkillLogName).Open();
		}
#endif
	}
}

void UGridChessPieceSkillComponent::OnUnregister()
{
	Super::OnUnregister();

	AbilitySystemComponent = nullptr;
}

void UGridChessPieceSkillComponent::OnChessPieceInitialized()
{
	const AGridChessPiece* Pawn = GetPawnChecked<AGridChessPiece>();

	if (const UGridChessPieceExtensionComponent* ChessPieceExtComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(Pawn))
	{
		AbilitySystemComponent = ChessPieceExtComponent->GetGridAbilitySystemComponent();
		HeroInfo = ChessPieceExtComponent->GetChessPieceInfo();
	}

	if (HeroInfo.IsValid() && AbilitySystemComponent)
	{
		for (const auto SkillInfoPair : HeroInfo->SkillInfoMap)
		{
			UGridSkillInfo* SkillInfo = SkillInfoPair.Value;
			UGridGameplayAbility_Card* AbilityCOD = SkillInfo->SkillData.Ability->GetDefaultObject<UGridGameplayAbility_Card>();

			FGameplayAbilitySpec AbilitySpec(AbilityCOD, SkillInfo->Level);
			AbilitySpec.SourceObject = nullptr;
			SkillInfo->AbilitySystemComponent = AbilitySystemComponent;
			SkillInfo->AbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}
}
