#pragma once
#include "TilemapEditor/Widget/Tilemap3DEditModeWidget.h"

class FTilemap3DEditorViewportClient;

class FTilemap3DBaseMode
{
public:
	virtual ~FTilemap3DBaseMode() = default;
	FTilemap3DBaseMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient);

	virtual void EnterMode() PURE_VIRTUAL(,)
	virtual void InputKey(const FInputKeyEventArgs& EventArgs) PURE_VIRTUAL(,)
	virtual void ExitMode()	PURE_VIRTUAL(,)
	virtual void ClearMode() {}
	
protected:
	TWeakPtr<FTilemap3DEditorViewportClient> ViewportClient;
};