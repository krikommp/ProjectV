// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DChessDataDetails.h"

#include "SlateOptMacros.h"
#include "ChessPieces/GridChessPieceData.h"
#include "TilemapEditor/Tilemap3DEditorToolkit.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STilemap3DChessDataDetails::Construct(const FArguments& InArgs, FTilemap3DEditorToolkit* InTilemapEditor, UGridChessPieceData* InChessPieceData)
{
	ChessPieceData = InChessPieceData;
	
	SSingleObjectDetailsPanel::Construct(
	SSingleObjectDetailsPanel::FArguments().HostCommandList(InTilemapEditor->GetToolkitCommands()).
											HostTabManager(InTilemapEditor->GetTabManager()),
	/*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/ true);
}

UObject* STilemap3DChessDataDetails::GetObjectToObserve() const
{
	return ChessPieceData.Get();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
