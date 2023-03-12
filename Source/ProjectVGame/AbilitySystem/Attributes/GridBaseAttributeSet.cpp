// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBaseAttributeSet.h"

#include "AbilitySystem/GridAbilitySystemComponent.h"

UGridBaseAttributeSet::UGridBaseAttributeSet()
{
}

UWorld* UGridBaseAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UGridAbilitySystemComponent* UGridBaseAttributeSet::GetGridAbilitySystemComponent() const
{
	return Cast<UGridAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
