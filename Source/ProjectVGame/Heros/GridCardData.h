// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DataTable.h"
#include "Templates/SubclassOf.h"
#include "GameplayTagContainer.h"
#include "GridCardData.generated.h"

class UTexture2D;
class UUserWidget;
class UGridGameplayAbility_Card;

USTRUCT(BlueprintType)
struct FGridCardAbilityDescriptionArg
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag GameplayTag;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridCardData : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** 卡牌标识 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	FString CardID;

	/** 卡牌名称 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	FText CardName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	FGameplayTag ContainerTag;

	/** 卡牌描述 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	FText CardDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	TArray<FGridCardAbilityDescriptionArg> AbilityEffectArgs;

	/** 卡面资产 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="GridCardData")
	TObjectPtr<UTexture2D> Portrait;

	/** 卡面边框资产 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="GridCardData")
	TObjectPtr<UTexture2D> Frame;

	/** 卡牌UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	TSubclassOf<UUserWidget> CardVisualWidget;

	/** 卡牌赋予的技能 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	TSubclassOf<UGridGameplayAbility_Card> GrantedAbility;

	// 卡牌默认技能等级
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GridCardData")
	int32 DefaultCardAbilityLevel = -1;
	
	bool operator==(const FGridCardData& Other) const
	{
		return CardID == Other.CardID;
	}

	bool operator!=(const FGridCardData& Other) const
	{
		return !(*this == Other);
	}
};
