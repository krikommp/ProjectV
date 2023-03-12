// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridBaseAttributeSet.h"
#include "UObject/Object.h"
#include "GridMoveSet.generated.h"

/**
 * UGridHealthSet
 *
 * 用于定义与伤害有关的属性
 * 该属性可以包含例如：生命，护盾，抗性
 */
UCLASS(BlueprintType)
class PROJECTVGAME_API UGridMoveSet : public UGridBaseAttributeSet
{
	GENERATED_BODY()

public:
	UGridMoveSet();

	ATTRIBUTE_ACCESSORS(UGridMoveSet, Acceleration);
	ATTRIBUTE_ACCESSORS(UGridMoveSet, MaxSpeed);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Acceleration(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxSpeed(const FGameplayAttributeData& OldValue);

private:
	/** 加速度 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Acceleration, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Acceleration;

	/** 最大速度 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxSpeed, Category = "Grid|Action", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSpeed;
};
