#pragma once
#include "Tilemap3DBaseMode.h"

class FTilemap3DPlayerStartMode final : public FTilemap3DBaseMode
{
public:
	FTilemap3DPlayerStartMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient);

	//~ Begin FTilemap3DBaseMode interface
	virtual void EnterMode() override;
	virtual void InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual void ExitMode() override;
	//~ Begin FTilemap3DBaseMode interface

	private:
	float HitResultTraceDistance;
};
