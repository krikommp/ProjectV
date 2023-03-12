// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridBaseAttributeSet.h"

#include "GridCombatSet.generated.h"

/**
 * UGridCombatSet
 *
 * 用于定义战斗有关的属性
 * 该属性可以包含：伤害值，治疗值等
 */
UCLASS(BlueprintType)
class PROJECTVGAME_API UGridCombatSet : public UGridBaseAttributeSet
{
	GENERATED_BODY()

public:
	UGridCombatSet();

	ATTRIBUTE_ACCESSORS(UGridCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UGridCombatSet, BaseHealing);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:

	/** 用于在伤害 execution 中计算 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Grid|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	/** 用于在治疗 execution 中计算 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Grid|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHealing;
};
