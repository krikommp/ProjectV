// Fill out your copyright notice in the Description page of Project Settings.


#include "GridHUDLayout.h"

#include "CommonUIExtensions.h"
#include "Delegates/Delegate.h"
#include "Input/CommonUIInputTypes.h"
#include "Misc/AssertionMacros.h"
#include "NativeGameplayTags.h"
#include "UITag.h"
#include "UObject/NameTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridHUDLayout)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");

UGridHUDLayout::UGridHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGridHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
}

void UGridHUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, EscapeMenuClass);
	}
}
