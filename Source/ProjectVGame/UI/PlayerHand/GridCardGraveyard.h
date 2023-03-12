// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardPileInterface.h"
#include "CommonUserWidget.h"
#include "GridCardGraveyard.generated.h"

class UGridCard;
class UGridPlayerHand;

/**
 * UGridCardGraveyard
 *
 * 玩家卡牌墓地
 * 所有打出的卡牌都会保存到这个Pile中
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridCardGraveyard : public UCommonUserWidget, public ICardPileInterface
{
	GENERATED_BODY()
public:
	UGridCardGraveyard(const FObjectInitializer& ObjectInitializer);

	// ~UserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// ~UserWidget interface
	
	void SetupCardGraveyard(UGridPlayerHand* InPlayerHand);

	// ~CardPileInterface
	virtual void AddCard(UGridCard* Card) override;
	virtual void RemoveCard(UGridCard* Card) override;
	virtual void SortCards(const TArray<UGridCard*>& InCards) override;
	virtual void ClearCard() override;
	// ~CardPileInterface

	FVector2D GetCardAnchorLocation() const;
private:
	void OnCardDiscard(UGridCard* PlayedCard);
	void NotifyPileChanged() const;

	FOnPileChanged OnPileChanged;
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUserWidget> CardAnchor;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|CardGraveyard")
	TObjectPtr<UGridPlayerHand> PlayerHand;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|CardGraveyard")
	TArray<UGridCard*> Cards;
};