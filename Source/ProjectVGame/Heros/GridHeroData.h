// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GridHeroData.generated.h"

class AGridChessBase;
/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridHeroData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 英雄ID
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	FName HeroID = FName();

	// 英雄名称
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	FText HeroName;

	// 英雄描述
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	FText Description;

	// 棋子类
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	TSubclassOf<AGridChessBase> ChessClass;

	// 蒙皮骨骼
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 动画蓝图
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	TSubclassOf<UAnimInstance> AnimBlueprint;

	// 英雄默认等级
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	int32 DefaultLevel = INDEX_NONE;
	
	// 默认卡牌组
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	TArray<FString> DefaultCardIDs;

	// 默认卡包名称
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	FText DefaultCardPackageName;

	// 默认技能列表
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	TArray<FName> DefaultSkillList;

	// 默认所属派系
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	FGameplayTag Faction;

	// 默认友好派系
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GridHeroData")
	//FGameplayTagContainer FriendlyFactions;
};
