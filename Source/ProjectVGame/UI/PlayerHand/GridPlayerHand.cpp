// Fill out your copyright notice in the Description page of Project Settings.

#include "GridPlayerHand.h"

#include "GridCardDrawPile.h"
#include "GridCardGraveyard.h"
#include "GridCardPlayPlane.h"
#include "GridCardSelectPlane.h"
#include "GridLogChannel.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Character/GridPlayerHandComponent.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Card/GridCard.h"
#include "UI/SubSystem/GridUICardManagerSubsystem.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

void UGridPlayerHand::NativeConstruct()
{
	Super::NativeConstruct();

	CardSelectPlane->SetupCardSelectPlane(this);
	CardPlayPlane->SetupCardPlayPlane(this);
	CardGraveyard->SetupCardGraveyard(this);
	CardDrawPile->SetupCardDrawPile(this);
	OnPileChanged.AddUObject(this, &ThisClass::SortCards);
}

void UGridPlayerHand::NativeDestruct()
{
	Super::NativeDestruct();

	OnPileChanged.RemoveAll(this);
}

void UGridPlayerHand::AddCard(UGridCard *Card)
{
	HandCanvas->AddChild(Card);
	FVector2D DrawCardPixelPosition, DrawCardViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), CardDrawPile->GetTickSpaceGeometry(), CardDrawPile->GetRenderTransform().Translation, DrawCardPixelPosition, DrawCardViewportPosition);
	FWidgetTransform NewCardTransform;
	NewCardTransform.Translation = DrawCardViewportPosition;
	NewCardTransform.Scale = FVector2D::Zero();
	NewCardTransform.Angle = 0.0f;
	Card->SetRenderTransform(NewCardTransform);

	Cards.Add(Card);
	NotifyCardPileChanged();
}

void UGridPlayerHand::RemoveCard(UGridCard *Card)
{
	if (UGridPlayerHandComponent *PlayerHandComponent = UGridPlayerHandComponent::FindPlayerHandComponent(UGameplayStatics::GetGameInstance(GetWorld())->GetPrimaryPlayerController()->GetPawn()))
	{
		Cards.Remove(Card);
		PlayerHandComponent->CardsHoldInHand.Remove(Card->CardInfo);
		NotifyCardPileChanged();
	}
}

TArray<FWidgetTransform> UGridPlayerHand::CalculateCardTransform(const TArray<UGridCard *> &InCards)
{
	TArray<FWidgetTransform> Result;
	const UGridUICardManagerSubsystem *UICardManagerSubsystem = GetGameInstance()->GetSubsystem<UGridUICardManagerSubsystem>();
	const float CardSpacingInHand = UICardManagerSubsystem->GetValue<float>("CardSpacingInHand");
	const float CardAngleFromCenter = UICardManagerSubsystem->GetValue<float>("CardAngleFromCenter");
	for (int32 CardIndex = 0; CardIndex < InCards.Num(); ++CardIndex)
	{
		const float CardMoveToX = CardSpacingInHand * CardIndex - ((CardSpacingInHand / 2.0f) * (InCards.Num() - 1)) + GetCardAnchorLocation().X;
		const float CardMoveToY = FMath::Square((CardIndex - (InCards.Num() - 1) * 0.5) * 2.0f) + GetCardAnchorLocation().Y;
		FWidgetTransform NewCardTransform;
		NewCardTransform.Translation = FVector2D(CardMoveToX, CardMoveToY);
		NewCardTransform.Scale = FVector2D::One();
		NewCardTransform.Angle = (CardAngleFromCenter * (-1.0f) * (InCards.Num() - 1)) + (CardAngleFromCenter * 2.0f * CardIndex);
		Result.Add(NewCardTransform);
	}
	return Result;
}

void UGridPlayerHand::SortCards(const TArray<UGridCard *> &InCards)
{
	TArray<FWidgetTransform> NewCardTransforms = CalculateCardTransform(InCards);
	check(NewCardTransforms.Num() == InCards.Num());
	for (int32 CardIndex = 0; CardIndex < InCards.Num(); ++CardIndex)
	{
		FWidgetTransform NewCardTransform = NewCardTransforms[CardIndex];
		InCards[CardIndex]->RequestDesiredTransformUpdate(NewCardTransform);
		InCards[CardIndex]->SuggestZOrder(CardIndex);
		InCards[CardIndex]->CardState = ECardState::Draw;
	}
}

void UGridPlayerHand::ClearCard()
{
	const UGridUICardManagerSubsystem *UICardManager = GetGameInstance()->GetSubsystem<UGridUICardManagerSubsystem>();
	for (const auto &Card : Cards)
	{
		UICardManager->ReturnCardWidget(Card);
	}
	Cards.Reset();
}

void UGridPlayerHand::Register_OnEnterCardSelectPlane(FOnEnterCardSelectPlane::FDelegate &&Delegate)
{
	if (!OnEnterCardSelectPlane.IsBoundToObject(Delegate.GetUObject()))
	{
		OnEnterCardSelectPlane.Add(MoveTemp(Delegate));
	}
}

void UGridPlayerHand::UnRegister_OnEnterCardSelectPlane(const UObject *InUser)
{
	OnEnterCardSelectPlane.RemoveAll(InUser);
}

void UGridPlayerHand::Register_OnEnterCardPlayPlane(FOnEnterCardSelectPlane::FDelegate &&Delegate)
{
	if (!OnEnterCardPlayPlane.IsBoundToObject(Delegate.GetUObject()))
	{
		OnEnterCardPlayPlane.Add(MoveTemp(Delegate));
	}
}

void UGridPlayerHand::UnRegister_OnEnterCardPlayPlane(const UObject *InUser)
{
	OnEnterCardPlayPlane.RemoveAll(InUser);
}

void UGridPlayerHand::Register_OnCardPlayPlaneMouseButtonDown(FOnCardPlayPlaneMouseButtonDown::FDelegate &&Delegate)
{
	if (!OnCardPlayPlaneMouseButtonDown.IsBoundToObject(Delegate.GetUObject()))
	{
		OnCardPlayPlaneMouseButtonDown.Add(MoveTemp(Delegate));
	}
}

void UGridPlayerHand::UnRegister_OnCardPlayPlaneMouseButtonDown(const UObject *InUser)
{
	OnCardPlayPlaneMouseButtonDown.RemoveAll(InUser);
}

void UGridPlayerHand::HoverCard(UGridCard *InCard)
{
	InCard->CardState = ECardState::Hover;
	for (const auto &Card : Cards)
	{
		if (Card != InCard)
		{
			FWidgetTransform NewTransform = Card->GetRenderTransform();
			NewTransform.Translation.X += 2.0f;
			Card->RequestDesiredTransformUpdate(NewTransform, true);
		}
	}
}

void UGridPlayerHand::PlaySelectCard()
{
	PlayCard(SelectedCard);
}

void UGridPlayerHand::UnSelectCard()
{
	UnSelectCard(SelectedCard);
}

void UGridPlayerHand::PlayCard(UGridCard *CardToPlay)
{
	CardToPlay->CardState = ECardState::Play;
	NotifyCardPlayed(CardToPlay);
}

void UGridPlayerHand::Discard(UGridCard *CardToDiscard)
{
	Cards.Remove(CardToDiscard);
	NotifyCardDiscard(CardToDiscard);
	NotifyCardPileChanged();
	SelectedCard = nullptr;
}

void UGridPlayerHand::SelectCard(UGridCard *InSelectedCard)
{
	SelectedCard = InSelectedCard;
	for (const auto &OtherCard : Cards)
	{
		if (OtherCard != SelectedCard)
		{
			OtherCard->CardState = ECardState::Disable;
		}
	}
	NotifyCardSelect(SelectedCard);
}

void UGridPlayerHand::UnSelectCard(UGridCard *UnSelectCard)
{
	SelectedCard = nullptr;
	NotifyCardUnSelect();
	NotifyCardPileChanged();
}

void UGridPlayerHand::PlayerHandShow(AActor *HandOwner)
{
	if (HandOwner == nullptr)
	{
		return;
	}

	if (HandOwner == CurrentHandOwner)
	{
		return;
	}

	CurrentHandOwner = HandOwner;
}

void UGridPlayerHand::GateAllCardsInputState()
{
	for (const auto &Card : Cards)
	{
		Card->GateCardInput();
	}
}

void UGridPlayerHand::ChangeAllCardsInHandState(ECardState NewCardState)
{
	for (const auto &Card : Cards)
	{
		Card->CardState = NewCardState;
	}
}

FVector2D UGridPlayerHand::GetCardAnchorLocation() const
{
	const auto Geometry = CardAnchor->GetTickSpaceGeometry();
	FVector2D PixelPosition, ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Geometry, CardAnchor->GetRenderTransform().Translation, PixelPosition, ViewportPosition);
	return ViewportPosition;
}

void UGridPlayerHand::NotifyEnterSelectCardPlane() const
{
	OnEnterCardSelectPlane.Broadcast();
	OnEnterCardSelectPlaneDynamic.Broadcast();
}

void UGridPlayerHand::NotifyLeaveSelectCardPlane() const
{
	OnLeaveCardSelectPlane.Broadcast();
	OnLeaveCardSelectPlaneDynamic.Broadcast();
}

void UGridPlayerHand::NotifyEnterPlayCardPlane() const
{
	OnEnterCardPlayPlane.Broadcast();
	OnEnterCardPlayPlaneDynamic.Broadcast();
}

void UGridPlayerHand::NotifyCardPlayed(UGridCard *PlayedCard) const
{
	OnCardPlayed.Broadcast(PlayedCard);
	OnCardPlayedDynamic.Broadcast(PlayedCard);
}

void UGridPlayerHand::NotifyCardDiscard(UGridCard *InCard) const
{
	OnCardDiscard.Broadcast(InCard);
	OnCardDiscardDynamic.Broadcast(InCard);
}

void UGridPlayerHand::NotifyCardHover(UGridCard *HoveredCard) const
{
	OnCardHover.Broadcast(HoveredCard);
	OnCardHoverDynamic.Broadcast(HoveredCard);
}

void UGridPlayerHand::NotifyCardUnHover() const
{
	OnCardUnHover.Broadcast();
	OnCardUnHoverDynamic.Broadcast();
}

void UGridPlayerHand::NotifyCardSelect(UGridCard *InSelectedCard) const
{
	OnCardSelected.Broadcast(InSelectedCard);
	OnCardSelectedDynamic.Broadcast(InSelectedCard);
}

void UGridPlayerHand::NotifyCardUnSelect() const
{
	OnCardUnSelected.Broadcast();
	OnCardUnSelectedDynamic.Broadcast();
}

void UGridPlayerHand::NotifyPlayCardPlaneMouseButtonDown(const FPointerEvent &InMouseEvent) const
{
	OnCardPlayPlaneMouseButtonDown.Broadcast(InMouseEvent);
	OnCardPlayPlaneMouseButtonDownDynamic.Broadcast(InMouseEvent);
}

void UGridPlayerHand::NotifyCardPileChanged() const
{
	OnPileChanged.Broadcast(Cards);
}