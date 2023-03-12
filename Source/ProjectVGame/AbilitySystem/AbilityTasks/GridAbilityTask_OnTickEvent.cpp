// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAbilityTask_OnTickEvent.h"

UGridAbilityTask_OnTickEvent::UGridAbilityTask_OnTickEvent(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UGridAbilityTask_OnTickEvent* UGridAbilityTask_OnTickEvent::AbilityTaskOnTick(UGameplayAbility* OwningAbility,
	FName TaskInstanceName)
{
	UGridAbilityTask_OnTickEvent* MyObj = NewAbilityTask<UGridAbilityTask_OnTickEvent>(OwningAbility);
	return MyObj;
}

void UGridAbilityTask_OnTickEvent::Activate()
{
	Super::Activate();
}

void UGridAbilityTask_OnTickEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	OnTickTask.Broadcast(DeltaTime);
	OnTickTaskDynamic.Broadcast(DeltaTime);
}

void UGridAbilityTask_OnTickEvent::TaskEnded(UObject* Owner)
{
	OnTickTask.RemoveAll(Owner);
	OnTickTaskDynamic.RemoveAll(Owner);
	EndTask();
}
