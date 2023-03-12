// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridBaseAttributeSet.h"
#include "UObject/Object.h"
#include "GridActionSet.generated.h"

/**
 * UGridActionSet
 *
 * 用于定义行动有关的属性
 * 该属性可以包含：行动力，速度
 */
UCLASS(BlueprintType)
class PROJECTVGAME_API UGridActionSet : public UGridBaseAttributeSet
{
	GENERATED_BODY()

public:
	UGridActionSet();

	ATTRIBUTE_ACCESSORS(UGridActionSet, ActionPoint);
	ATTRIBUTE_ACCESSORS(UGridActionSet, MaxActionPoint);
	ATTRIBUTE_ACCESSORS(UGridActionSet, MoveCostRatio);
	ATTRIBUTE_ACCESSORS(UGridActionSet, MovePreStepCost);
	ATTRIBUTE_ACCESSORS(UGridActionSet, EveryTurnDrawCardsNum);
	ATTRIBUTE_ACCESSORS(UGridActionSet, VisibleRange);
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ActionPoint(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxActionPoint(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnReq_MoveCostRatio(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnReq_MovePreStepCost(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnReq_EveryTurnDrawCardsNum(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnReq_VisibleRange(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
private:

	/** 单位行动力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ActionPoint, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ActionPoint;

	/** 最大行动力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxActionPoint, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxActionPoint;

	/** 移动消耗行动力的比例，默认为 1 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnReq_MoveCostRatio, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveCostRatio;

	/** 每步移动消耗，默认为 1 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnReq_MovePreStepCost, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovePreStepCost;

	// 每回合抽牌数量，默认为 3
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnReq_EveryTurnDrawCardsNum, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EveryTurnDrawCardsNum;

	// 单位可视范围, 一般来说可视范围大于可移动范围
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnReq_VisibleRange, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData VisibleRange;
};
