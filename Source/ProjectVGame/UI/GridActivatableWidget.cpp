// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActivatableWidget.h"

#include "Editor/WidgetCompilerLog.h"
#include "CommonInputModeTypes.h"
#include "Input/UIActionBindingHandle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridActivatableWidget)

#define LOCTEXT_NAMESPACE "Grid"

UGridActivatableWidget::UGridActivatableWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UGridActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EGridWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode, false, MouseLockMode, bShowCursor);
	case EGridWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode, false, MouseLockMode, bShowCursor);
	case EGridWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EGridWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

#if WITH_EDITOR
void UGridActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
	IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UGridActivatableWidget, BP_GetDesiredFocusTarget)))
	{
		if (GetParentNativeClass(GetClass()) == UGridActivatableWidget::StaticClass())
		{
			CompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			CompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
		}
	}
}
#endif

#undef LOCTEXT_NAMESPACE