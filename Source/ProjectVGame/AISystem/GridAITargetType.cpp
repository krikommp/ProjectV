// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAITargetType.h"

#include "ChessPieces/GridChessPieceExtensionComponent.h"

int32 UGridAITargetType::GetTargets_Implementation(const UObject* WorldContextObject, AActor* OwnerActor, const TArray<int32>& SearchableIndexArray)
{
	return INDEX_NONE;
}

int32 UGridAITargetType_User::GetTargets_Implementation(const UObject* WorldContextObject, AActor* OwnerActor, const TArray<int32>& SearchableIndexArray)
{
	if (const UGridChessPieceExtensionComponent* ChessPieceExtComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(OwnerActor))
	{
		return ChessPieceExtComponent->GetTileIndex();
	}
	return INDEX_NONE;
}
