#pragma once
#include "TilemapEditor/Widget/Tilemap3DEditModeWidget.h"

class FTilemap3DEditorViewportClient;

class FTilemap3DBaseMode
{
public:
	virtual ~FTilemap3DBaseMode() = default;
	FTilemap3DBaseMode();
	bool CheckEditMode(const ETilemap3DEditMode InEditMode) const { return EditMode == InEditMode; }

	virtual void InputKey(FTilemap3DEditorViewportClient* ViewportClient, const FInputKeyEventArgs& EventArgs) PURE_VIRTUAL(,)

protected:
	ETilemap3DEditMode EditMode = UNKNOWN;
};

#define CHECK_EDITMODE(InViewPortClient) \
	if (!CheckEditMode((InViewPortClient)->GetEditMode())) \
	{ \
		return; \
	}
