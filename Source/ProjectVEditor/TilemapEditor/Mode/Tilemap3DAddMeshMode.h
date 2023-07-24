#pragma once
#include "Tilemap3DBaseMode.h"

class FTilemap3DAddMeshMode final : public FTilemap3DBaseMode
{
public:
	FTilemap3DAddMeshMode();

	//~ Begin FTilemap3DBaseMode interface
	virtual void InputKey(FTilemap3DEditorViewportClient* ViewportClient, const FInputKeyEventArgs& EventArgs) override;
	//~ Begin FTilemap3DBaseMode interface
private:
	float HitResultTraceDistance;
};
