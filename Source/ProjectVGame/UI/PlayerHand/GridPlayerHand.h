// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardPileInterface.h"
#include "UI/GridActivatableWidget.h"
#include "UI/GridHUDLayout.h"
#include "UI/Card/GridCard.h"
#include "GridPlayerHand.generated.h"

class UGridCardDrawPile;
class UGridCardGraveyard;
class UGridCardInfo;
class UCanvasPanel;
class UGridCardSelectPlane;
class UGridCardPlayPlane;

DECLARE_MULTICAST_DELEGATE(FOnEnterCardSelectPlane);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterCardSelectPlaneDynamic);
DECLARE_MULTICAST_DELEGATE(FOnLeaveCardSelectPlane);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaveCardSelectPlaneDynamic);
DECLARE_MULTICAST_DELEGATE(FOnEnterCardPlayPlane);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterCardPlayPlaneDynamic);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardPlayPlaneMouseButtonDown, const FPointerEvent& InMouseEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardPlayPlaneMouseButtonDownDynamic,  const FPointerEvent&, InMouseEvent);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardDiscard, UGridCard* InCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardDiscardDynamic, UGridCard*, InCard);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardPlayed, UGridCard* PlayedCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardPlayedDynamic, UGridCard*, InCard);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardHover, UGridCard* HoveredCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardHoverDynamic, UGridCard*, HoveredCard);
DECLARE_MULTICAST_DELEGATE(FOnCardUnHover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardUnHoverDynamic);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardSelected, UGridCard* SelectedCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelectedDynamic, UGridCard*, SelectedCard);
DECLARE_MULTICAST_DELEGATE(FOnCardUnSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardUnSelectedDynamic);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerHandMotionEnded);

/**
 * UGridPlayerHand
 *
 * 手牌类，用于在对战中展示当前玩家抽到的手牌
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridPlayerHand : public UGridHUDLayout, public ICardPileInterface
{
	GENERATED_BODY()

	friend class UGridCardSelectPlane;
	friend class UGridCardPlayPlane;
	friend class UGridCardDrawPile;
	friend class UGridCardGraveyard;
	friend class UGridPlayerHandStateComponent;

public:

	// ~UserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// ~UserWidget interface
	
	// ~CardPileInterface
	virtual void AddCard(UGridCard* Card) override;
	virtual void RemoveCard(UGridCard* Card) override;
	virtual void SortCards(const TArray<UGridCard*>& InCards) override;
	virtual void ClearCard() override;
	// ~CardPileInterface

	// ~Register or UnRegister Events
	void Register_OnEnterCardSelectPlane(FOnEnterCardSelectPlane::FDelegate&& Delegate);
	void UnRegister_OnEnterCardSelectPlane(const UObject* InUser);
	void Register_OnEnterCardPlayPlane(FOnEnterCardSelectPlane::FDelegate&& Delegate);
	void UnRegister_OnEnterCardPlayPlane(const UObject* InUser);
	void Register_OnCardPlayPlaneMouseButtonDown(FOnCardPlayPlaneMouseButtonDown::FDelegate&& Delegate);
	void UnRegister_OnCardPlayPlaneMouseButtonDown(const UObject* InUser);
	// ~Register or UnRegister Events

	// ~PlayerHand Card Operator Functions
	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	FWidgetTransform HoverCard(UGridCard* InCard);

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void UnHoverCard(UGridCard* InCard);
	
	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void PlaySelectCard();

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void UnSelectCard();
	void UnSelectCard(UGridCard* UnSelectCard);
	
	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void PlayCard(UGridCard* CardToPlay);

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void Discard(UGridCard* CardToDiscard);

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void SelectCard(UGridCard* InSelectedCard);
	// ~PlayerHand Card Operator Functions

	UFUNCTION(BlueprintImplementableEvent, Category="Grid|PlayerHand")
	void OnPlayerHandShow();
	void PlayerHandShow(AActor* HandOwner);

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void GateAllCardsInputState();

	UFUNCTION(BlueprintCallable)
	void ChangeAllCardsInHandState(ECardState NewCardState);

	// 获取卡牌中心锚点位置
	UFUNCTION(BlueprintPure, Category="Grid|PlayerHand")
	FVector2D GetCardAnchorLocation() const;

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void NotifyCardHover(UGridCard* HoveredCard) const;

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void NotifyCardUnHover() const;

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void NotifyCardUnSelect() const;
private:
	void NotifyCardPileChanged() const;
	void NotifyEnterSelectCardPlane() const;
	void NotifyLeaveSelectCardPlane() const;
	void NotifyEnterPlayCardPlane() const;
	void NotifyCardPlayed(UGridCard* PlayedCard) const;
	void NotifyCardDiscard(UGridCard* InCard) const;
	void NotifyCardSelect(UGridCard* InSelectedCard) const;
	void NotifyPlayCardPlaneMouseButtonDown(const FPointerEvent& InMouseEvent) const;

	TArray<FWidgetTransform> CalculateCardTransform(const TArray<UGridCard*>& InCards);
public:
	// 抽牌堆
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UGridCardDrawPile> CardDrawPile;

	// 弃牌堆
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UGridCardGraveyard> CardGraveyard;

	// 选牌区域
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UGridCardSelectPlane> CardSelectPlane;

	// 打牌区域
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UGridCardPlayPlane> CardPlayPlane;
protected:

	// 卡牌锚点
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UUserWidget> CardAnchor;

	// 手牌界面
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> HandCanvas;
	
public:
	// 进入打牌区域事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnEnterCardPlayPlane"))
	FOnEnterCardPlayPlaneDynamic OnEnterCardPlayPlaneDynamic;

	// 进入选牌区域事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnEnterCardSelectPlane"))
	FOnEnterCardSelectPlaneDynamic OnEnterCardSelectPlaneDynamic;

	// 离开选牌区域事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnLeaveCardSelectPlane"))
	FOnLeaveCardSelectPlaneDynamic OnLeaveCardSelectPlaneDynamic;

	// 卡牌选中事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardHover"))
	FOnCardHoverDynamic OnCardHoverDynamic;

	// 卡牌取消选中事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardUnHover"))
	FOnCardUnHoverDynamic OnCardUnHoverDynamic;

	// 卡牌选中事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardSelected"))
	FOnCardSelectedDynamic OnCardSelectedDynamic;

	// 卡牌取消选中事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardUnSelected"))
	FOnCardUnSelectedDynamic OnCardUnSelectedDynamic;

	// 卡牌打出事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardPlayed"))
	FOnCardPlayedDynamic OnCardPlayedDynamic;

	// 卡牌丢弃事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardDiscard"))
	FOnCardDiscardDynamic OnCardDiscardDynamic;

	// 卡牌点击事件
	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand", meta=(DisplayName="OnCardPlayPlaneMouseButtonDown"))
	FOnCardPlayPlaneMouseButtonDownDynamic OnCardPlayPlaneMouseButtonDownDynamic;

	UPROPERTY(BlueprintAssignable, Category="Grid|PlayerHand")
	FOnPlayerHandMotionEnded OnPlayerHandMotionEnded;
protected:
	// 当前持有手牌对象
	UPROPERTY(BlueprintReadWrite, Category="Grid|PlayerHand")
	TObjectPtr<AActor> CurrentHandOwner;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|PlayerHand")
	TArray<UGridCard*> Cards;

	FOnPileChanged OnPileChanged;
	FOnEnterCardSelectPlane OnEnterCardSelectPlane;
	FOnLeaveCardSelectPlane OnLeaveCardSelectPlane;
	FOnEnterCardPlayPlane OnEnterCardPlayPlane;
	FOnCardPlayPlaneMouseButtonDown OnCardPlayPlaneMouseButtonDown;
	FOnCardPlayed OnCardPlayed;
	FOnCardDiscard OnCardDiscard;
	FOnCardHover OnCardHover;
	FOnCardUnHover OnCardUnHover;
	FOnCardSelected OnCardSelected;
	FOnCardUnSelected OnCardUnSelected;

public:
	// 当前选中的卡牌对象，可能为空
	TObjectPtr<UGridCard> SelectedCard;
};
