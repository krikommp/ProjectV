// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCardDrawPile.h"

#include "GridPlayerHand.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/GridPlayerHandComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/Card/GridCard.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SubSystem/GridUICardManagerSubsystem.h"

UGridCardDrawPile::UGridCardDrawPile(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UGridCardDrawPile::SetupCardDrawPile(UGridPlayerHand* InPlayerHand)
{
	if (InPlayerHand == PlayerHand)
	{
		return;
	}
	PlayerHand = InPlayerHand;
}

void UGridCardDrawPile::AddCard(UGridCard* Card)
{
}

void UGridCardDrawPile::RemoveCard(UGridCard* Card)
{
}

void UGridCardDrawPile::SortCards(const TArray<UGridCard*>& InCards)
{
}

void UGridCardDrawPile::ClearCard()
{
}

FVector2D UGridCardDrawPile::GetCardAnchorLocation() const
{
	const auto Geometry = CardAnchor->GetTickSpaceGeometry();
	FVector2D PixelPosition, ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Geometry, CardAnchor->GetRenderTransform().Translation, PixelPosition, ViewportPosition);
	return ViewportPosition;
}

UGridCard* UGridCardDrawPile::DrawCard()
{
	// todo...
	ensureAlwaysMsgf(PlayerHand, TEXT("InValid Player Hand, Run Setup function."));
	// 首先从卡牌对象池中获取一个卡牌对象
	const UGridUICardManagerSubsystem* UICardManagerSubsystem = GetGameInstance()->GetSubsystem<UGridUICardManagerSubsystem>();
	UGridCard* NewCard = UICardManagerSubsystem->GetCardWidget();

	// 将这个新的卡牌对象加入到Canvas中
	PlayerHand->HandCanvas->AddChild(NewCard);

	// 根据配置设置卡牌大小
	UCanvasPanelSlot* CardCanvas = UWidgetLayoutLibrary::SlotAsCanvasSlot(NewCard);
	NewCard->SetCardSize(UICardManagerSubsystem->GetValue<FVector2D>("CardSizeInHand"));
	CardCanvas->SetSize(UICardManagerSubsystem->GetValue<FVector2D>("CardSizeInHand"));
	CardCanvas->SetAlignment(FVector2D(0.5, 0.5));

	// 设置初始值
	NewCard->PlayerHand = PlayerHand;

	// 设置卡牌位置信息
	FWidgetTransform NewTransform;
	NewTransform.Translation = GetCardAnchorLocation();
	NewTransform.Scale = FVector2D::Zero();
	NewTransform.Angle = 0.0f;
	NewCard->SetRenderTransform(NewTransform);
	NewCard->SetRenderOpacity(UICardManagerSubsystem->GetValue<float>("CardDisableOpacity"));

	// 初始化卡牌状态机
	NewCard->InitializeStateMachine();

	// 开始执行
	NewCard->CardState = ECardState::Init;
	NewCard->DoStart();
	
	return NewCard;
}

TArray<UGridCard*> UGridCardDrawPile::DrawCardFromPlayerHand()
{
	ensureAlwaysMsgf(PlayerHand, TEXT("InValid Player Hand, Run Setup function."));
	
	TArray<UGridCard*> OutCards;
	if (UGridPlayerHandComponent* PlayerHandComponent = UGridPlayerHandComponent::FindPlayerHandComponent(UGameplayStatics::GetGameInstance(GetWorld())->GetPrimaryPlayerController()->GetPawn()))
	{
		for (const auto CardInfo : PlayerHandComponent->CardsHoldInHand)
		{
			auto Card = DrawCard();
			Card->SetupCardInfo(CardInfo);
			OutCards.Add(Card);
		}
	}

	return OutCards;
}