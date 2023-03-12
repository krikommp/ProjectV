// Fill out your copyright notice in the Description page of Project Settings.


#include "GridConfirmationScreen.h"

#include "CommonBorder.h"
#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "GridButtonBase.h"
#include "ICommonInputModule.h"
#include "Components/DynamicEntryBox.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

void UGridConfirmationScreen::SetupDialog(UCommonGameDialogDescriptor* Descriptor,
                                          FCommonMessagingResultDelegate ResultCallback)
{
	Super::SetupDialog(Descriptor, ResultCallback);

	Text_Tile->SetText(Descriptor->Header);
	RichText_Description->SetText(Descriptor->Body);

	EntryBox_Buttons->Reset<UGridButtonBase>([](UGridButtonBase& Button)
	{
		Button.OnClicked().Clear();
	});

	for(const FConfirmationDialogAction& Action : Descriptor->ButtonActions)
	{
		FDataTableRowHandle ActionRow;

		switch(Action.Result)
		{
		case ECommonMessagingResult::Confirmed:
			ActionRow = ICommonInputModule::GetSettings().GetDefaultClickAction();
			break;
		case ECommonMessagingResult::Declined:
			ActionRow = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;
		case ECommonMessagingResult::Cancelled:
			ActionRow = CancelAction;
			break;
		default:
			ensure(false);
			continue;
		}

		UGridButtonBase* Button = EntryBox_Buttons->CreateEntry<UGridButtonBase>();
		Button->SetTriggeredInputAction(ActionRow);
		Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
		Button->SetButtonText(Action.OptionalDisplayText);
	}

	OnResultCallback = ResultCallback;
}

void UGridConfirmationScreen::KillDialog()
{
	Super::KillDialog();
}

void UGridConfirmationScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Border_TapToCloseZone->OnMouseButtonDownEvent.BindDynamic(this, &UGridConfirmationScreen::HandleTapToCloseZoneMouseButtonDown);
}

void UGridConfirmationScreen::CloseConfirmationWindow(ECommonMessagingResult Result)
{
	DeactivateWidget();
	OnResultCallback.ExecuteIfBound(Result);
}

FEventReply UGridConfirmationScreen::HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = FReply::Unhandled();

	if (MouseEvent.IsTouchEvent() || MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CloseConfirmationWindow(ECommonMessagingResult::Declined);
		Reply.NativeReply = FReply::Handled();
	}

	return Reply;
}

#if WITH_EDITOR
void UGridConfirmationScreen::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	if (CancelAction.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has unset property: CancelAction.")), FText::FromString(GetName())));
	}
}

#endif




