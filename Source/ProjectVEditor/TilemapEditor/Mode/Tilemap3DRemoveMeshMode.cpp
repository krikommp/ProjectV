#include "Tilemap3DRemoveMeshMode.h"

FTilemap3DRemoveMeshMode::FTilemap3DRemoveMeshMode()
	: FTilemap3DBaseMode(), HitResultTraceDistance(10000.0f)
{
	EditMode = EEM_Mesh_Remove;
}

void FTilemap3DRemoveMeshMode::EnterMode()
{
}

void FTilemap3DRemoveMeshMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
                                        const FInputKeyEventArgs& EventArgs)
{
	
}

void FTilemap3DRemoveMeshMode::ExitMode()
{
}
