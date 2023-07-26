#include "Tilemap3DSpawnChessMode.h"

FTilemap3DSpawnChessMode::FTilemap3DSpawnChessMode()
	: HitResultTraceDistance(10000.0), ChessID(FName())
{
	EditMode = EEM_Chess_Spawn;
}

void FTilemap3DSpawnChessMode::EnterMode()
{
}

void FTilemap3DSpawnChessMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
	const FInputKeyEventArgs& EventArgs)
{
}

void FTilemap3DSpawnChessMode::ExitMode()
{
}
