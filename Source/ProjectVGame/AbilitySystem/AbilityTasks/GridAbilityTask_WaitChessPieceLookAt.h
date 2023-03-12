// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GridAbilityTask_WaitChessPieceLookAt.generated.h"

class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridWaitChessPieceLookAt);

/**
 * UGridAbilityTask_WaitChessPieceLookAt
 *
 * 等待棋子面向指定目标
 */
UCLASS()
class PROJECTVGAME_API UGridAbilityTask_WaitChessPieceLookAt : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(BlueprintAssignable)
	FGridWaitChessPieceLookAt OnWaitChessPieceLookAt;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UGridAbilityTask_WaitChessPieceLookAt* WaitChessPieceLookAt(UGameplayAbility* OwningAbility, int32 InTargetIndex);

	// ~ UGameplayTask
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	// ~ UGameplayTask
	
protected:

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void OnFinishLookAt();

	float GetChessPieceMaxSpeed() const;
	float GetChessPieceAcceleration() const;

private:
	bool bStartMovement;
	FRotator TargetRotator;
	int32 TargetIndex;
	float Speed;
};
