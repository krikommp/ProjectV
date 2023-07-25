#pragma once
#include "TilemapEditor/Widget/Tilemap3DEditModeWidget.h"

class FTilemap3DEditorViewportClient;

class FTilemap3DBaseMode
{
public:
	virtual ~FTilemap3DBaseMode() = default;
	FTilemap3DBaseMode();
	bool CheckEditMode(const ETilemap3DEditMode InEditMode) const { return EditMode == InEditMode; }

	virtual void EnterMode() PURE_VIRTUAL(,)
	virtual void InputKey(FTilemap3DEditorViewportClient* ViewportClient, const FInputKeyEventArgs& EventArgs) PURE_VIRTUAL(,)
	virtual void ExitMode()	PURE_VIRTUAL(,)
	virtual void ClearMode() {}
	
protected:
	ETilemap3DEditMode EditMode = UNKNOWN;
};

#define CHECK_EDITMODE(InViewPortClient) \
	if (!CheckEditMode((InViewPortClient)->EditMode.Get())) \
	{ \
		return; \
	}
