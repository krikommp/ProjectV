// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGlobalAbilitySystem.h"

#include "GridAbilitySystemComponent.h"

UGridGlobalAbilitySystem::UGridGlobalAbilitySystem()
{
}

void UGridGlobalAbilitySystem::RegisterASC(UGridAbilitySystemComponent* ASC)
{
	check(ASC);

	RegisteredASCs.AddUnique(ASC);
}

void UGridGlobalAbilitySystem::UnregisterASC(UGridAbilitySystemComponent* ASC)
{
	check(ASC);

	RegisteredASCs.Remove(ASC);
}
