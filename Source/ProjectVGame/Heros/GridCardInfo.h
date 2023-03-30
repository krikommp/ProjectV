// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GridCardData.h"
#include "AbilitySystem/AbilityEvents/GridAbilityInputEvent.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"

#include "GridCardInfo.generated.h"

class UGridLocalPlayer;
class UGridGameplayAbility_Card;
class UGridAbilitySystemComponent;
class AGridChessPiece;

/**
 * UGridCardInfo
 *
 * 储存卡牌数据，当添加一张新卡牌时会生成一份保存数据
 * 用于被 SaveGame 保存和加载
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTVGAME_API UGridCardInfo : public USaveGame
{
	GENERATED_BODY()

	friend class UGridPlayerHandComponent;
public:
	UGridCardInfo();

	void SaveCardInfoFlush();
	static UGridCardInfo* CreateOrLoadCardInfo(const UGridLocalPlayer* LocalPlayer, const FString& CardId, const FGridCardData& CardData);
	
	void Initialize(UGridLocalPlayer* LocalPlayer);
	
	FString GetSaveSlotName() const;
	bool IsValid() const { return CardID.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category="Grid|CardInfo")
	UGridGameplayAbility_Card* GetCardAbilityInstance(UGridAbilitySystemComponent* InAbilitySystemComponent) const;

	UFUNCTION(BlueprintPure, Category="Grid|Card")
	FORCEINLINE UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }

	UFUNCTION(BlueprintPure, Category="Grid|Card")
	bool CheckAbilityCanActivate(FGameplayTagContainer& OptionalRelevantTags) const;

	UFUNCTION(BlueprintCallable, Category="Grid|Card")
	bool TryActivateCardAbility();

	UFUNCTION(BlueprintCallable, Category="Grid|Card")
	void TryCancelCardAbility();

	UFUNCTION(BlueprintCallable, Category="Grid|Card")
	void SendCardInputEvent(const FGridAbilityInputEvent& InputEvent);

	UFUNCTION(BlueprintPure, Category="Grid|Card")
	FText GetCardDescription();
public:
	// 卡牌ID，用以区分相同类型的卡牌
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|CardInfo")
	FString CardID;

	// 卡牌类型ID，可以用来在配置表中查找
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|CardInfo")
	FString CardTypeID;

	// 卡牌技能等级
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grid|CardInfo")
	int32 AbilityLevel;

public:
	// 拥有卡牌的英雄对象
	UPROPERTY(BlueprintReadOnly, Transient, Category="Grid|CardInfo")
	const AGridChessPiece* OwningHero = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Grid|CardInfo")
	FGridCardData CardData;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Grid|CardInfo")
	UGridAbilitySystemComponent* AbilitySystemComponent;

private:
	UPROPERTY(Transient)
	UGridLocalPlayer* OwningPlayer = nullptr;

	FGameplayAbilitySpecHandle GrantedHandle;
};
