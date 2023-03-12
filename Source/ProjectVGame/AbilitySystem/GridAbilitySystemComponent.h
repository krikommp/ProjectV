// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilityEvents/GridAbilityEvent.h"
#include "AbilityEvents/GridAbilityInputEvent.h"

#include "GridAbilitySystemComponent.generated.h"

class UGridGameplayAbility_Card;
class UGridAbilityBuffUIData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardAbilityEnded, UGridGameplayAbility_Card*, InCardAbility, bool, bWasCancelled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityEvent, const FGameplayAbilitySpecHandle&, InAbilitySpecHandle, const FGridAbilityEvent&, AbilityEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGridAbilityInputEvent, const FGameplayAbilitySpecHandle&, InAbilitySpecHandle, const FGridAbilityInputEvent&, CardAbilityInputEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityBuffActive, const UGridAbilityBuffUIData*, BuffUIData, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityBuffRefresh, const UGridAbilityBuffUIData*, BuffUIData, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityBuffRemoved, const UGridAbilityBuffUIData*, BuffUIData);

/**
 * UGridAbilitySystemComponent
 *
 * 项目自定义 AbilitySystemComponent
 */
UCLASS()
class PROJECTVGAME_API UGridAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UGridAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~UActorComponent interface
	
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void TickAbilityTurn(int32 Delta);

	UFUNCTION(BlueprintCallable, Category="Grid|Buff")
	TMap<UGridAbilityBuffUIData*, float> GetAllActiveBuffInfos();

protected:

	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void OnGameplayEffectDurationChange(FActiveGameplayEffect& ActiveEffect) override;
	
	void NotifyGameplayEffectActivate(UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle);
	void NotifyGameplayEffectRemoved(const FActiveGameplayEffect&);
	void NotifyGameplayEffectRefresh(const FActiveGameplayEffect&);

public:

	UPROPERTY(BlueprintAssignable, Category="Grid|Card Ability")
	FOnCardAbilityEnded OnCardAbilityEnded;

	UPROPERTY(BlueprintAssignable, Category="Grid|Card Ability")
	FOnAbilityEvent OnAbilityEvent;

	UPROPERTY(BlueprintAssignable, Category="Grid|Card Ability")
	FOnGridAbilityInputEvent OnGridAbilityInputEvent;

	UPROPERTY(BlueprintAssignable, Category="Grid|Buff")
	FOnAbilityBuffActive OnAbilityBuffActive;

	UPROPERTY(BlueprintAssignable, Category="Grid|Buff")
	FOnAbilityBuffRefresh OnAbilityBuffRefresh;

	UPROPERTY(BlueprintAssignable, Category="Grid|Buff")
	FOnAbilityBuffRemoved OnAbilityBuffRemoved;
};
