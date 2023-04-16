// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardPileInterface.h"
#include "CommonUserWidget.h"
#include "GridCardDrawPile.generated.h"

class UGridCard;
class UGridPlayerHand;
class UGridCardInfo;

/**
 * UGridCardDrawPile
 *
 * 玩家抽牌堆
 * 所有待抽取的卡牌都保存在这个类中
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridCardDrawPile : public UCommonUserWidget, public ICardPileInterface
{
	GENERATED_BODY()

public:
	UGridCardDrawPile(const FObjectInitializer& ObjectInitializer);
	
	void SetupCardDrawPile(UGridPlayerHand* InPlayerHand);

	// ~CardPileInterface
	virtual void AddCard(UGridCard* Card) override;
	virtual void RemoveCard(UGridCard* Card) override;
	virtual void SortCards(const TArray<UGridCard*>& InCards) override;
	virtual void ClearCard() override;
	// ~CardPileInterface

	UFUNCTION(BlueprintCallable, Category="Grid|CardDrawPile")
	TArray<UGridCard*> DrawCardFromPlayerHand();
	
	void OnNewCardAddToHand(UGridCardInfo* InNewCardInfo);

protected:
	// 抽一张牌
	UGridCard* DrawCard();

	// 获取锚点位置
	FVector2D GetCardAnchorLocation() const;
	
protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|CardDrawPile")
	TObjectPtr<UGridPlayerHand> PlayerHand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUserWidget> CardAnchor;
};
