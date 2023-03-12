// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridHeroData.h"
#include "GameFramework/SaveGame.h"

#include "GridHeroInfo.generated.h"

class UGridLocalPlayer;
class UGridCardPackageInfo;
class UGridSkillInfo;

/**
 * UGridHeroInfo
 *
 * 用于保存的 HeroInfo
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTVGAME_API UGridHeroInfo : public USaveGame
{
	GENERATED_BODY()

public:
	UGridHeroInfo();

	void SaveHeroInfoFlush();
	static UGridHeroInfo* LoadOrCreateHeroInfo(const FGridHeroData& HeroData, const UGridLocalPlayer* LocalPlayer);
	static UGridHeroInfo* CreateHeroInfo(const FGridHeroData& HeroData);

	void Initialize(UGridLocalPlayer* InLocalPlayer, bool bDefault);

	FString GetSaveSlotName() const;
	bool IsValid() const { return HeroID != INDEX_NONE; }

	UGridCardPackageInfo* GetCurrentCardPackage() { return HoldingCardPackages.FindChecked(UsingCardPackageID); }

public:
	// 英雄ID
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grid|Hero")
	int32 HeroID;

	// 英雄等级
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grid|Hero")
	int32 HeroLevel;

	// 当前选择使用的卡包索引
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grid|Hero")
	FString UsingCardPackageID;

	// 所有可供使用的卡包索引队列
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grid|Hero")
	TArray<FString> HoldingCardPackageIDs;

	// 可供使用的技能列表
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grid|Hero")
	TArray<FName> SkillList;

public:
	// 英雄数据
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grid|Hero")
	FGridHeroData HeroData;

	// 持有的卡包
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grid|Hero")
	TMap<FString, TObjectPtr<UGridCardPackageInfo>> HoldingCardPackages;

	// 持有技能
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grid|Hero")
	TMap<FName, UGridSkillInfo*> SkillInfoMap;

	// 运行时友好联盟
	FGameplayTagContainer RuntimeFriendlyFactions;

	// 运行时所属派系
	UPROPERTY(Transient, BlueprintReadWrite, Category="Grid|Hero")
	FGameplayTag RuntimeFaction;
protected:
	UPROPERTY(Transient)
	TObjectPtr<const UGridLocalPlayer> OwningPlayer;
};
