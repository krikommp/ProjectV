// Fill out your copyright notice in the Description page of Project Settings.


#include "GridUIMessaging.h"
#include "Messaging/CommonGameDialog.h"
#include "NativeGameplayTags.h"
#include "Player/GridLocalPlayer.h"
#include "PrimaryGameLayout.h"
#include "Templates/Casts.h"
#include "UObject/NameTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridUIMessaging)

class FSubsystemCollectionBase;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");

void UGridUIMessaging::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ConfirmationDialogClassPtr = ConfirmationDialogClass.LoadSynchronous();
	ErrorDialogClassPtr = ErrorDialogClass.LoadSynchronous();
}

void UGridUIMessaging::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (UGridLocalPlayer* LocalPlayer = GetLocalPlayer<UGridLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			});
		}
	}
}

void UGridUIMessaging::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (UGridLocalPlayer* LocalPlayer = GetLocalPlayer<UGridLocalPlayer>())
	{
		if (UPrimaryGameLayout* RootLayout = LocalPlayer->GetRootUILayout())
		{
			RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ErrorDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			});
		}
	}
}