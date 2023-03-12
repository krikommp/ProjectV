// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GridAbilityTask_OnTickEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDynamicDelegate, float, DeltaTime);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float DeltaTime);

/**
 * UGridAbilityTask_OnTickEvent
 *
 * Tick 事件
 */
UCLASS()
class PROJECTVGAME_API UGridAbilityTask_OnTickEvent : public UAbilityTask
{
	GENERATED_BODY()
public:

	UGridAbilityTask_OnTickEvent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGridAbilityTask_OnTickEvent* AbilityTaskOnTick(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName);
	
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	void TaskEnded(UObject* Owner);

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDynamicDelegate OnTickTaskDynamic;

	FOnTickTaskDelegate OnTickTask;

private:
	float TaskDeltaTime;
};
