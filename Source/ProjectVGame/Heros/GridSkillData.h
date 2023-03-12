// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "GridSkillData.generated.h"

class UGridGameplayAbility_Card;

/**
 * FGridSkillData
 *
 * 技能数据
 */
USTRUCT(BlueprintType)
struct PROJECTVGAME_API FGridSkillData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGridGameplayAbility_Card> Ability;
};
