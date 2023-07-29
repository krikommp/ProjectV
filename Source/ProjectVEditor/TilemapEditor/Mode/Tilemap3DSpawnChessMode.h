#pragma once
#include "Tilemap3DBaseMode.h"

class FTilemap3DSpawnChessMode final : public FTilemap3DBaseMode
{
public:
	FTilemap3DSpawnChessMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient);

	//~ Begin FTilemap3DBaseMode interface
	virtual void EnterMode() override;
	virtual void InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual void ExitMode() override;
	//~ Begin FTilemap3DBaseMode interface
	
private:
	float HitResultTraceDistance;
	int32 BlockIndex;
};
