#pragma once
#include "CommonInputModeTypes.h"
#include "GameplayTagContainer.h"
#include "ChessPieces/GridChessPiece.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAvtiveInputModeChanged, ECommonInputMode);
DECLARE_MULTICAST_DELEGATE(FOnChessPieceTurnOver);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChessPieceSelectChanged, AGridChessPiece* /* OldFaction */, AGridChessPiece* /* NewFaction */);

class PROJECTVGAME_API FGridGlobalDelegates
{
public:
	static FOnAvtiveInputModeChanged OnActiveInputModeChanged;

	static FOnChessPieceTurnOver OnChessPieceTurnOver;

	static FOnChessPieceSelectChanged OnChessPieceSelectChanged;
};
