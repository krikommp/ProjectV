#pragma once
#include "Tilemap3DBaseMode.h"

class FTilemap3DSpawnChessMode final : public FTilemap3DBaseMode
{
public:
	FTilemap3DSpawnChessMode();

	//~ Begin FTilemap3DBaseMode interface
	virtual void EnterMode() override;
	virtual void InputKey(FTilemap3DEditorViewportClient* ViewportClient, const FInputKeyEventArgs& EventArgs) override;
	virtual void ExitMode() override;
	//~ Begin FTilemap3DBaseMode interface
	
private:
	float HitResultTraceDistance;
	FName ChessID;
};
