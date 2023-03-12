// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameplayCueManager.h"

#include "AbilitySystemGlobals.h"

UGridGameplayCueManager::UGridGameplayCueManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UGridGameplayCueManager* UGridGameplayCueManager::Get()
{
	return Cast<UGridGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

void UGridGameplayCueManager::LoadAlwaysLoadedCues()
{
}
