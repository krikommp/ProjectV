// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CardPileInterface.generated.h"

class UGridCard;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPileChanged, const TArray<UGridCard*>& InCards);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCardPileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ICardPileInterface
 *
 * 描述一堆卡牌的接口
 * 手牌，墓地，抽牌堆都需要继承于该接口
 */
class PROJECTVGAME_API ICardPileInterface
{
	GENERATED_BODY()

public:
	// 向牌堆中添加一张卡
	UFUNCTION(BlueprintCallable, Category="Grid|CardPile")
	virtual void AddCard(UGridCard* Card) = 0;

	// 将指定卡牌从卡堆中移除
	UFUNCTION(BlueprintCallable, Category="Grid|CardPile")
	virtual void RemoveCard(UGridCard* Card) = 0;

	// 清空卡堆中所有的卡牌
	UFUNCTION(BlueprintCallable, Category="Grid|CardPile")
	virtual void ClearCard() = 0;

	// 对输入的卡组进行排序
	UFUNCTION(BlueprintCallable, Category="Grid|CardPile")
	virtual void SortCards(const TArray<UGridCard*>& InCards) = 0;
};
