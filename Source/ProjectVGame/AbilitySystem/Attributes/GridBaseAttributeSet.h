// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GridBaseAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UGridAbilitySystemComponent;

// 用于广播属性事件的委托
DECLARE_MULTICAST_DELEGATE_FourParams(FGridAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);

/**
 * UGridBaseAttributeSet
 *
 * 基础的 AttributeSet，其他的 AttributeSet 需要从这里继承
 */
UCLASS()
class PROJECTVGAME_API UGridBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGridBaseAttributeSet();

	UWorld* GetWorld() const override;

	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const;
};
