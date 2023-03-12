// Fill out your copyright notice in the Description page of Project Settings.


#include "GridButtonBase.h"

#include "CommonActionWidget.h"
#include "GridLogChannel.h"

void UGridButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateButtonStyle();
	RefreshButtonText();
}

void UGridButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	UpdateButtonStyle();
	RefreshButtonText();
}

void UGridButtonBase::SetButtonText(const FText& InText)
{
	bOverride_ButtonText = InText.IsEmpty();
	ButtonText = InText;
	RefreshButtonText();
}

void UGridButtonBase::RefreshButtonText()
{
	if (bOverride_ButtonText || ButtonText.IsEmpty())
	{
		if (InputActionWidget)
		{
			const FText ActionDisplayText = InputActionWidget->GetDisplayText();
			if (!ActionDisplayText.IsEmpty())
			{
				UpdateButtonText(ActionDisplayText);
				return;
			}
		}
	}
	UpdateButtonText(ButtonText);
}

void UGridButtonBase::TestFunc()
{
	UE_LOG(LogGrid, Error, TEXT("Name = [%s]"), *GetNameSafe(GetOwningPlayer()));
}

void UGridButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);

	UpdateButtonStyle();
}
