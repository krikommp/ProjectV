// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityEvents/GridAbilityInputEvent.h"
#include "Components/PawnComponent.h"
#include "GridPlayerHandComponent.generated.h"

class AGridChessPiece;
class UGridCardInfo;
class UGridCard;

/**
 * 
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridPlayerHandComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGridPlayerHandComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	static UGridPlayerHandComponent* FindPlayerHandComponent(const AActor* Actor) { return Actor ? Actor->FindComponentByClass<UGridPlayerHandComponent>() : nullptr; }

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	TArray<UGridCardInfo*> GetRandomCards(int32 CardNum);

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerHand")
	void MoveCardToGraveyard(UGridCardInfo* InCardInfo);

	UFUNCTION(BlueprintPure, Category="Grid|PlayerHand")
	UGridCard* GetCurrentCard() const;
	
protected:
	virtual void OnRegister() override;
	
	void OnChessPieceSpawnFinished(const TArray<AGridChessPiece*>& InChessPieceArray);
	void OnTurnStart();
	void OnTurnFinish();

	void RandomCardsInDrawCard();

	UGridCardInfo* DrawCard();

public:

	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|PlayerHand")
	TArray<UGridCardInfo*> CardsHoldInDraw;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|PlayerHand")
	TArray<UGridCardInfo*> CardsHoldInHand;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|PlayerHand")
	TArray<UGridCardInfo*> CardsHoldInGraveyard;
	
private:

	FRandomStream DrawCardsRandom;
};
