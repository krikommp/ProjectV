// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridHeroData.h"
#include "GameFramework/SaveGame.h"

#include "GridCardPackageInfo.generated.h"

class UGridCardInfo;
class UGridLocalPlayer;

DECLARE_DYNAMIC_DELEGATE_OneParam(FGridCardPackageNewCardToAddDynamicDelegate, UGridCardInfo*, AddedCard);
DECLARE_DYNAMIC_DELEGATE(FGridCardPackageRemoveCardDynamicDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FGridCardPackageNewCardToAddDelegate, UGridCardInfo* /* AddedCard */);
DECLARE_MULTICAST_DELEGATE(FGridCardPackageRemoveCardDelegate);

/**
 * UGridCardPackageInfo
 *
 * 角色卡包数据
 * 用来保存卡牌数据
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTVGAME_API UGridCardPackageInfo : public USaveGame
{
	GENERATED_BODY()

public:
	UGridCardPackageInfo();

	void SaveCardPackageFlush();
	static UGridCardPackageInfo* CreateOrLoadCardPackage(const UGridLocalPlayer* LocalPlayer, const FGridHeroData& HeroData, const FString& CardPackageID);

	void Initialize(UGridLocalPlayer* LocalPlayer);

	FString GetSaveSlotName() const;

public:

	// 添加一张新的卡牌到卡包中
	void AddNewCardToCardPackage(UGridCardInfo* CardToAdd);

	// 从卡包中移除一张卡牌
	void RemoveCardFromCardPackage(UGridCardInfo* CardToRemove);
public:

	// 卡包拥有者ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grid|CardPackage")
	int32 OwnerID;

	// 卡包ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grid|CardPackage")
	FString ID;

	// 卡包显示名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grid|CardPackage")
	FText DisplayName;

	// 卡包中拥有的卡牌ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grid|CardPackage")
	TMap<FString, FString> OwningCardIDLists;

public:

	// 卡包中拥有的卡牌
	UPROPERTY(Transient)
	TArray<UGridCardInfo*> OwningCardList;

	// 当新卡牌被加入卡包时调用委托
	FGridCardPackageNewCardToAddDelegate OnNewCardAdded;

	// 当有卡牌被从卡包中移除时调用委托
	FGridCardPackageRemoveCardDelegate OnRemoveCard;
private:

	UPROPERTY(Transient)
	UGridLocalPlayer* OwningPlayer;
};
