// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerHandComponent.h"

#include "GridGlobalDelegates.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"
#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceExtensionComponent.h"
#include "ChessPieces/GridChessPieceSpawnComponent.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameState.h"
#include "GridTurnManager/GridTurnManagerComponent.h"
#include "Heros/GridCardInfo.h"
#include "Heros/GridCardPackageInfo.h"
#include "Heros/GridHeroInfo.h"
#include "Kismet/GameplayStatics.h"
#include "System/GridGameInstance.h"
#include "UI/PlayerHand/GridPlayerHandStateComponent.h"

UGridPlayerHandComponent::UGridPlayerHandComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

TArray<UGridCardInfo*> UGridPlayerHandComponent::GetRandomCards(int32 CardNum)
{
	TArray<UGridCardInfo*> OutCards;
	if (CardNum <= CardsHoldInDraw.Num())
	{
		for (int32 i = 0; i < CardNum; ++i)
		{
			OutCards.Add(DrawCard());
		}
	}else if (!CardsHoldInGraveyard.IsEmpty())
	{
		CardsHoldInDraw.Append(CardsHoldInGraveyard);
		CardsHoldInGraveyard.Reset();
		RandomCardsInDrawCard();
		OutCards = GetRandomCards(CardNum);
	}else
	{
		const int32 MinCardNum = FMath::Min(CardNum, CardsHoldInDraw.Num());
		OutCards = GetRandomCards(MinCardNum);
	}
	return OutCards;
}

void UGridPlayerHandComponent::MoveCardToGraveyard(UGridCardInfo* InCardInfo)
{
	check(CardsHoldInHand.Contains(InCardInfo));
	CardsHoldInHand.Remove(InCardInfo);
	CardsHoldInGraveyard.Add(InCardInfo);

	if (InCardInfo->AbilitySystemComponent)
	{
		const FGameplayAbilitySpecHandle& SpecHandle = InCardInfo->GrantedHandle;
		InCardInfo->AbilitySystemComponent->SetRemoveAbilityOnEnd(SpecHandle);
	}
}

void UGridPlayerHandComponent::AddNewCardToPlayerHand(UGridCardInfo* InNewCardInfo)
{
	check(InNewCardInfo);
	
	// add gameplay ability
	UGridGameplayAbility_Card* CardAbilityCOD = InNewCardInfo->CardData.GrantedAbility->GetDefaultObject<UGridGameplayAbility_Card>();
	FGameplayAbilitySpec CardAbilitySpec(CardAbilityCOD, InNewCardInfo->AbilityLevel);
	CardAbilitySpec.SourceObject = nullptr;
	InNewCardInfo->GrantedHandle = InNewCardInfo->AbilitySystemComponent->GiveAbility(CardAbilitySpec);

	CardsHoldInHand.Add(InNewCardInfo);

	FGridGlobalDelegates::OnGridNewCardAdded.Broadcast(InNewCardInfo);
}

UGridCard* UGridPlayerHandComponent::GetCurrentCard() const
{
	if (UGridPlayerHandStateComponent* PlayerHandStateComponent = GetWorld()->GetGameState()->FindComponentByClass<UGridPlayerHandStateComponent>())
	{
		if (UGridPlayerHand* PlayerHand = PlayerHandStateComponent->GetPlayerHand())
		{
			return PlayerHand->SelectedCard;
		}
	}
	return nullptr;
}

void UGridPlayerHandComponent::OnRegister()
{
	Super::OnRegister();

	if (UGridChessPieceSpawnComponent* SpawnComponent = GetWorld()->GetGameState()->FindComponentByClass<UGridChessPieceSpawnComponent>())
	{
		SpawnComponent->CallOrRegister_OnChessPieceSpawnFinished(FOnGridChessPieceSpawnFinished::FDelegate::CreateUObject(this, &ThisClass::OnChessPieceSpawnFinished));
	}

	if (UGridTurnManagerComponent* TurnManagerComponent = GetWorld()->GetGameState()->FindComponentByClass<UGridTurnManagerComponent>())
	{
		TurnManagerComponent->OnTurnStart.Add(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnTurnStart));
		TurnManagerComponent->OnTurnFinish.Add(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnTurnFinish));
	}
}

void UGridPlayerHandComponent::OnChessPieceSpawnFinished(const TArray<AGridChessPiece*>& InChessPieceArray)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	const APlayerController* PlayerController = GameInstance->GetPrimaryPlayerController(false);

	const FDateTime Timestamp = FDateTime::Now();
	const uint32 Seed = Timestamp.ToUnixTimestamp();
	DrawCardsRandom.Initialize(Seed);

	for (const auto& ChessPiece : InChessPieceArray)
	{
		if (const UGridChessPieceExtensionComponent* ChessPieceExtensionComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(ChessPiece))
		{
			const APlayerController* ChessPieceOwnerController = UGameplayStatics::GetPlayerController(GetWorld(), ChessPieceExtensionComponent->GetPlayerIndex());
			if (ChessPieceOwnerController != nullptr && PlayerController == ChessPieceOwnerController)
			{
				UGridHeroInfo* PieceInfo = ChessPieceExtensionComponent->GetChessPieceInfo();
				for (const auto& CardInfo : PieceInfo->GetCurrentCardPackage()->OwningCardList)
				{
					CardInfo->OwningHero = ChessPiece;
					CardInfo->AbilitySystemComponent = ChessPieceExtensionComponent->GetGridAbilitySystemComponent();
					CardsHoldInDraw.Add(CardInfo);
				}
			}
		}
	}

	RandomCardsInDrawCard();
}

void UGridPlayerHandComponent::OnTurnStart()
{
	// todo...
	const auto CardInfos = GetRandomCards(5);

	for (const auto& CardInfo : CardInfos)
	{
		check(CardInfo->AbilitySystemComponent);
		
		CardsHoldInHand.Add(CardInfo);

		UGridGameplayAbility_Card* CardAbilityCOD = CardInfo->CardData.GrantedAbility->GetDefaultObject<UGridGameplayAbility_Card>();

		FGameplayAbilitySpec CardAbilitySpec(CardAbilityCOD, CardInfo->AbilityLevel);
		CardAbilitySpec.SourceObject = nullptr;
		CardInfo->GrantedHandle = CardInfo->AbilitySystemComponent->GiveAbility(CardAbilitySpec);
	}
}

void UGridPlayerHandComponent::OnTurnFinish()
{
	for (const auto& CardInfo : CardsHoldInHand)
	{
		if (CardInfo->AbilitySystemComponent)
		{
			CardInfo->AbilitySystemComponent->ClearAbility(CardInfo->GrantedHandle);
		}
	}
	
	CardsHoldInGraveyard.Append(CardsHoldInHand);
	CardsHoldInHand.Empty();
}

void UGridPlayerHandComponent::RandomCardsInDrawCard()
{
	for (int32 Index = 0; Index < CardsHoldInDraw.Num(); ++Index)
	{
		const int32 RandomIndex = DrawCardsRandom.RandRange(0, CardsHoldInDraw.Num() - 1);
		CardsHoldInDraw.Swap(Index, RandomIndex);
	}
}

UGridCardInfo* UGridPlayerHandComponent::DrawCard()
{
	if (CardsHoldInDraw.Num() > 0)
	{
		return CardsHoldInDraw.Pop();
	}
	return nullptr;
}
