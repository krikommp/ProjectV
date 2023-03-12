

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/AbilityEvents/GridAbilityEvent.h"
#include "AbilitySystem/AbilityEvents/GridAbilityInputEvent.h"
#include "GridAbilityTask_WaitAbilityEvent.generated.h"

class UGridGameplayAbility_Card;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridWaitAbilityEventDelegate, const FGridAbilityEvent&, AbilityEvent);

/**
 * 等待获取Ability事件
 */
UCLASS()
class PROJECTVGAME_API UGridAbilityTask_WaitAbilityEvent : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FGridWaitAbilityEventDelegate OnAbilityEvent;

	virtual void Activate() override;

	UFUNCTION()
	void OnReceiveAbilityEvent(const FGameplayAbilitySpecHandle& InAbilitySpecHandle, const FGridAbilityEvent& InAbilityEvent);

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UGridAbilityTask_WaitAbilityEvent* WaitAbilityEvent(UGameplayAbility* OwningAbility);

protected:

	virtual void OnDestroy(bool bInOwnerFinished) override;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridWaitCardAbilityInputDelegate, const FGridAbilityInputEvent&, CardAbilityInputEvent);

UCLASS()
class PROJECTVGAME_API UGridAbilityTask_WaitCardAbilityInputEvent : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FGridWaitCardAbilityInputDelegate OnGridAbilityInputEvent;

	virtual void Activate() override;

	UFUNCTION()
	void OnReceiveAbilityEvent(const FGameplayAbilitySpecHandle& InAbilitySpecHandle, const FGridAbilityInputEvent& InAbilityEvent);

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UGridAbilityTask_WaitCardAbilityInputEvent* WaitCardAbilityInputEvent(UGameplayAbility* OwningAbility);

protected:

	virtual void OnDestroy(bool bInOwnerFinished) override;	
};
