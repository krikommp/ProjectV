// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridChessPieceHelper.generated.h"

class AGridChessPiece;

/**
 * UGridChessPieceHelper
 *
 * 针对 ChessPiece 的辅助函数
 */
UCLASS()
class PROJECTVGAME_API UGridChessPieceHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Grid")
	static void SortChessPieceArray(UPARAM(ref) TArray<AGridChessPiece*>& ChessPieceList);
};
