#include "GridAbilityTask_WaitAbilityEvent.h"

#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/GridGameplayAbility_Card.h"

UGridAbilityTask_WaitAbilityEvent::UGridAbilityTask_WaitAbilityEvent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UGridAbilityTask_WaitAbilityEvent* UGridAbilityTask_WaitAbilityEvent::WaitAbilityEvent(UGameplayAbility* OwningAbility)
{
	UGridAbilityTask_WaitAbilityEvent* MyObj = NewAbilityTask<UGridAbilityTask_WaitAbilityEvent>(OwningAbility);
	return MyObj;
}

void UGridAbilityTask_WaitAbilityEvent::Activate()
{
	UGridAbilitySystemComponent* ASC = Cast<UGridAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->OnAbilityEvent.AddDynamic(this, &ThisClass::OnReceiveAbilityEvent);
	}
}

void UGridAbilityTask_WaitAbilityEvent::OnDestroy(bool bInOwnerFinished)
{
	UGridAbilitySystemComponent* ASC = Cast<UGridAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->OnAbilityEvent.RemoveDynamic(this, &ThisClass::OnReceiveAbilityEvent);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGridAbilityTask_WaitAbilityEvent::OnReceiveAbilityEvent(const FGameplayAbilitySpecHandle& InAbilitySpecHandle, const FGridAbilityEvent& InAbilityEvent)
{
	if (Ability->GetCurrentAbilitySpecHandle() == InAbilitySpecHandle)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnAbilityEvent.Broadcast(InAbilityEvent);
		}
		
		EndTask();
	}
}

UGridAbilityTask_WaitCardAbilityInputEvent::UGridAbilityTask_WaitCardAbilityInputEvent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UGridAbilityTask_WaitCardAbilityInputEvent* UGridAbilityTask_WaitCardAbilityInputEvent::WaitCardAbilityInputEvent(
	UGameplayAbility* OwningAbility)
{
	UGridAbilityTask_WaitCardAbilityInputEvent* MyObj = NewAbilityTask<UGridAbilityTask_WaitCardAbilityInputEvent>(OwningAbility);
	return MyObj;
}

void UGridAbilityTask_WaitCardAbilityInputEvent::OnDestroy(bool bInOwnerFinished)
{
	UGridAbilitySystemComponent* ASC = Cast<UGridAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->OnGridAbilityInputEvent.RemoveDynamic(this, &ThisClass::OnReceiveAbilityEvent);
	}

	Super::OnDestroy(bInOwnerFinished);
}


void UGridAbilityTask_WaitCardAbilityInputEvent::Activate()
{
	UGridAbilitySystemComponent* ASC = Cast<UGridAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->OnGridAbilityInputEvent.AddDynamic(this, &ThisClass::OnReceiveAbilityEvent);
	}
}

void UGridAbilityTask_WaitCardAbilityInputEvent::OnReceiveAbilityEvent(
	const FGameplayAbilitySpecHandle& InAbilitySpecHandle, const FGridAbilityInputEvent& InAbilityEvent)
{
	if (Ability->GetCurrentAbilitySpecHandle() == InAbilitySpecHandle)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnGridAbilityInputEvent.Broadcast(InAbilityEvent);
		}
		
		EndTask();
	}
}



