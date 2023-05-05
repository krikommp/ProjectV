// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapNode.h"
#include "ChessPieces/GridChessPiece.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GridGameplayTags.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"

AGridMapNode::AGridMapNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UGridAbilitySystemComponent>(
		this, TEXT("AbilitySystemComponent"));
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

void AGridMapNode::OnChessPieceEnter(AGridChessPiece* InChessPiece) const
{
	const FGridGameplayTags GridGameplayTags = FGridGameplayTags::Get();
	FGameplayEventData EventData;
	EventData.Instigator = Owner;
	EventData.Target = InChessPiece;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GridGameplayTags.GameplayEvent_OnChessEnterTile,
	                                                         FGameplayEventData());
}

void AGridMapNode::OnChessPieceLeave(AGridChessPiece* InChessPiece) const
{
	const FGridGameplayTags GridGameplayTags = FGridGameplayTags::Get();
	FGameplayEventData EventData;
	EventData.Instigator = Owner;
	EventData.Target = InChessPiece;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GridGameplayTags.GameplayEvent_OnChessLeaveTile,
															 FGameplayEventData());
}
