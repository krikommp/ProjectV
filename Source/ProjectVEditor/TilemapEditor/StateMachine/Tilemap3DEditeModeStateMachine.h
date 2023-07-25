#pragma once
#include "TilemapEditor/Tilemap3DEditorManager.h"

class FTilemap3DEditorViewportClient;
class FTilemap3DBaseMode;

class FTilemap3DEditeModeStateMachine
{
public:
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<FTilemap3DBaseMode, T>>>
	void RegisterEditMode(const ETilemap3DEditMode EditMode)
	{
		if (EditeModeStates.Contains(EditMode))
		{
			UE_LOG(LogTemp, Error, TEXT("%d already in state list."), EditMode);
			return;
		}
		TSharedPtr<T> NewState = MakeShareable(new T);
		EditeModeStates.Add(EditMode, NewState);
	}

	void ChangeState(const ETilemap3DEditMode NewState);
	void InputKey(FTilemap3DEditorViewportClient* ViewportClient, const FInputKeyEventArgs& EventArgs) const;
	void ClearState();

private:
	TMap<const ETilemap3DEditMode, TSharedPtr<FTilemap3DBaseMode>> EditeModeStates;
	TWeakPtr<FTilemap3DBaseMode> CurrentState = nullptr;
};
