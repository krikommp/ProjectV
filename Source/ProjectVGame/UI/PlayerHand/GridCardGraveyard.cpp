// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCardGraveyard.h"

#include "GridLogChannel.h"
#include "GridPlayerHand.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Character/GridPlayerHandComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Card/GridCard.h"
#include "UI/SubSystem/GridUICardManagerSubsystem.h"

UGridCardGraveyard::UGridCardGraveyard(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGridCardGraveyard::NativeConstruct()
{
	Super::NativeConstruct();

	OnPileChanged.AddUObject(this, &ThisClass::SortCards);
}

void UGridCardGraveyard::NativeDestruct()
{
	Super::NativeDestruct();

	OnPileChanged.RemoveAll(this);
}

void UGridCardGraveyard::SetupCardGraveyard(UGridPlayerHand* InPlayerHand)
{
	if (PlayerHand == InPlayerHand)
	{
		return;
	}
	PlayerHand = InPlayerHand;

	PlayerHand->OnCardDiscard.Add(FOnCardDiscard::FDelegate::CreateUObject(this, &ThisClass::OnCardDiscard));
}

void UGridCardGraveyard::AddCard(UGridCard* Card)
{
	if (UGridPlayerHandComponent* PlayerHandComponent = UGridPlayerHandComponent::FindPlayerHandComponent(UGameplayStatics::GetGameInstance(GetWorld())->GetPrimaryPlayerController()->GetPawn()))
	{
		PlayerHandComponent->MoveCardToGraveyard(Card->CardInfo);
		Cards.Add(Card);
		NotifyPileChanged();
		Card->CardState = ECardState::Discard;
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("Can not get CardComponent From Current Unit [%s], please spawn this on his body.."), *GetNameSafe(PlayerHand->CurrentHandOwner));
	}
}

void UGridCardGraveyard::RemoveCard(UGridCard* Card)
{
	if (Cards.Remove(Card) != INDEX_NONE )
	{
		GetGameInstance()->GetSubsystem<UGridUICardManagerSubsystem>()->ReturnCardWidget(Card);
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("Can not found card [%s] in card graveyard."), *GetNameSafe(Card));
	}
}

void UGridCardGraveyard::SortCards(const TArray<UGridCard*>& InCards)
{
	ensureAlwaysMsgf(PlayerHand, TEXT("InValid Player Hand, Run Setup function."));
	for (const auto& Card : InCards)
	{
		FWidgetTransform NewTransform;
		NewTransform.Translation = GetCardAnchorLocation();
		NewTransform.Scale = FVector2D::Zero();
		NewTransform.Angle = 0.0f;
		Card->RequestDesiredTransformUpdate(NewTransform);
	}
}

void UGridCardGraveyard::ClearCard()
{
	for (const auto& Card : Cards)
	{
		GetGameInstance()->GetSubsystem<UGridUICardManagerSubsystem>()->ReturnCardWidget(Card);
	}
}

FVector2D UGridCardGraveyard::GetCardAnchorLocation() const
{
	const auto Geometry = CardAnchor->GetTickSpaceGeometry();
	FVector2D PixelPosition, ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Geometry, CardAnchor->GetRenderTransform().Translation, PixelPosition, ViewportPosition);
	return ViewportPosition;
}

void UGridCardGraveyard::OnCardDiscard(UGridCard* PlayedCard)
{
	AddCard(PlayedCard);
}

void UGridCardGraveyard::NotifyPileChanged() const
{
	if (OnPileChanged.IsBound())
	{
		OnPileChanged.Broadcast(Cards);
	}
}
