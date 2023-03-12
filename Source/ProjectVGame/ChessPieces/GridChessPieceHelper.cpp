// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceHelper.h"

#include "GridChessPiece.h"
#include "GridChessPieceExtensionComponent.h"

void UGridChessPieceHelper::SortChessPieceArray(TArray<AGridChessPiece*>& ChessPieceList)
{
	Algo::SortBy(ChessPieceList, [](AGridChessPiece* A)
	{
		const UGridChessPieceExtensionComponent* ChessPieceExtensionComponent = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(A);
		check(ChessPieceExtensionComponent);

		const int32 PlayerIndex = ChessPieceExtensionComponent->GetPlayerIndex();
		if (PlayerIndex == INDEX_NONE) return TNumericLimits<int32>::Max();

		return PlayerIndex;
	});
}
