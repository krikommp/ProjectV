#pragma once
#include "CommonInputModeTypes.h"
#include "GameplayTagContainer.h"
#include "ChessPieces/GridChessPiece.h"

class UGridCardInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAvtiveInputModeChanged, ECommonInputMode);
DECLARE_MULTICAST_DELEGATE(FOnChessPieceTurnOver);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChessPieceSelectChanged, AGridChessPiece* /* OldFaction */, AGridChessPiece* /* NewFaction */);
DECLARE_MULTICAST_DELEGATE(FOnChessPieceActionOver);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridNewCardAdded, UGridCardInfo* /* InNewCardInfo */)

class PROJECTVGAME_API FGridGlobalDelegates
{
public:
	static FOnAvtiveInputModeChanged OnActiveInputModeChanged;

	static FOnChessPieceTurnOver OnChessPieceTurnOver;

	static FOnChessPieceSelectChanged OnChessPieceSelectChanged;

	static FOnChessPieceActionOver OnChessPieceActionOver;

	static FOnGridNewCardAdded OnGridNewCardAdded;
};
