// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSingleObjectDetailsPanel.h"
#include "Widgets/SCompoundWidget.h"

class UGridChessData;
class FTilemap3DEditorToolkit;
/**
 * 
 */
class PROJECTVEDITOR_API STilemap3DChessDataDetails  : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(STilemap3DChessDataDetails)
		:_ChessPieceData(nullptr)
		{
		}
	SLATE_ATTRIBUTE(TObjectPtr<UGridChessData>, ChessPieceData)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, FTilemap3DEditorToolkit* InTilemapEditor);

	virtual UObject* GetObjectToObserve() const override;

private:
	TAttribute<TObjectPtr<UGridChessData>> ChessPieceData;
};
