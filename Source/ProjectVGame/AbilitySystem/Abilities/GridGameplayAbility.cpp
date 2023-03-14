// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameplayAbility.h"

#include "GridGlobalDelegates.h"

void UGridGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bWasCancelled)
	{
		FGridGlobalDelegates::OnChessPieceActionOver.Broadcast();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
