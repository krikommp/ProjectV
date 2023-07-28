#include "Tilemap3DEditeModeStateMachine.h"

#include "TilemapEditor/Mode/Tilemap3DBaseMode.h"

void FTilemap3DEditeModeStateMachine::ChangeState(const ETilemap3DEditMode NewState)
{
	if (!EditeModeStates.Contains(NewState))
	{
		UE_LOG(LogTemp, Warning, TEXT("NewState %d is not registered. Please re-register it."), static_cast<int32>(NewState));
		if (CurrentState != nullptr)
		{
			CurrentState.Pin()->ExitMode();
			CurrentState = nullptr;
		}
		return;
	}
	TWeakPtr<FTilemap3DBaseMode> State = EditeModeStates[NewState];
	if (CurrentState != nullptr && CurrentState != State)
	{
		CurrentState.Pin()->ExitMode();
	}
	CurrentState = EditeModeStates[NewState];
	if (CurrentState != nullptr)
	{
		CurrentState.Pin()->EnterMode();
	}
}

void FTilemap3DEditeModeStateMachine::InputKey(const FInputKeyEventArgs& EventArgs) const
{
	if (CurrentState != nullptr)
	{
		CurrentState.Pin()->InputKey(EventArgs);
	}
}

void FTilemap3DEditeModeStateMachine::ClearState()
{
	if (CurrentState != nullptr)
	{
		CurrentState.Pin()->ExitMode();
	}
	for (const auto [Mode, State] :  EditeModeStates)
	{
		State->ClearMode();
	}
	EditeModeStates.Empty();
}
	