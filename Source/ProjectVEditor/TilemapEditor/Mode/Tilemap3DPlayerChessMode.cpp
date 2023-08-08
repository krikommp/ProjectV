#include "Tilemap3DPlayerChessMode.h"

#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DPlayerChessMode::FTilemap3DPlayerChessMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0)
{
}

void FTilemap3DPlayerChessMode::EnterMode()
{
	FTilemap3DBaseMode::EnterMode();
}

void FTilemap3DPlayerChessMode::InputKey(const FInputKeyEventArgs& EventArgs)
{
	FTilemap3DBaseMode::InputKey(EventArgs);
}

void FTilemap3DPlayerChessMode::ExitMode()
{
	FTilemap3DBaseMode::ExitMode();
}