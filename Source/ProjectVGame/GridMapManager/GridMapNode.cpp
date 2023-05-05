// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapNode.h"

#include "AbilitySystem/GridAbilitySystemComponent.h"

AGridMapNode::AGridMapNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ObjectInitializer.CreateDefaultSubobject<UGridAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
}

void AGridMapNode::InitializeAbilitySystem()
{
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AGridMapNode::SetNodePosition(const FVector& NewPosition)
{
	SetActorLocation(NewPosition);
}
