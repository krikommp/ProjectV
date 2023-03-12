// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridBaseAttributeSet.h"
#include "UObject/Object.h"
#include "GridHealthSet.generated.h"

/**
 * UGridHealthSet
 *
 * 用于定义与伤害有关的属性
 * 该属性可以包含例如：生命，护盾，抗性
 */
UCLASS(BlueprintType)
class PROJECTVGAME_API UGridHealthSet : public UGridBaseAttributeSet
{
	GENERATED_BODY()

public:
	UGridHealthSet();

	ATTRIBUTE_ACCESSORS(UGridHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UGridHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UGridHealthSet, Defense);
	ATTRIBUTE_ACCESSORS(UGridHealthSet, Healing);

	/** 生命值到达0时广播该委托 */
	mutable FGridAttributeEvent OnOutOfHealth;

	/** 护盾值到达0时广播该委托 */
	mutable FGridAttributeEvent OnOutOfDefense;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
private:

	/** 当前生命值属性，会被最大生命值 clamp. health 不会显示在 modifiers 中，所以只能通过 executions 修改 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Grid|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** 当前最大生命值属性，可以被 GameplayEffect 直接修改 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category="Grid|MaxHealth", meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxHealth;

	/** 当前防御点数，可以被 GameplayEffect 直接修改 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category="Grid|Defense", meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Defense;

	/** 追踪生命值到达 0 */
	bool bOutOfHealth;

private:
	// meta attribute

	/** 治疗值, +Health */
	UPROPERTY(BlueprintReadOnly, Category="Grid|Health", meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	/** 伤害值，-Health */
	UPROPERTY(BlueprintReadOnly, Category="Grid|Health", meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
