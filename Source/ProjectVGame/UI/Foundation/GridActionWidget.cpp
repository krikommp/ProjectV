// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActionWidget.h"

#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"

FSlateBrush UGridActionWidget::GetIcon() const
{
	// 如果设置了 InputAction, 那么就从默认配置文件中获取输入图标
	if (AssociatedInputAction)
	{
		UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubSystem = GetEnhancedInputSubsystem();
		TArray<FKey> BoundKeys = EnhancedInputSubSystem->QueryKeysMappedToAction(AssociatedInputAction);
		FSlateBrush SlateBrush;

		if (!BoundKeys.IsEmpty() && UCommonInputPlatformSettings::Get()->TryGetInputBrush(SlateBrush, BoundKeys[0], CommonInputSubsystem->GetCurrentInputType(), CommonInputSubsystem->GetCurrentGamepadName()))
		{
			return SlateBrush;
		}
	}
	return Super::GetIcon();
}

UEnhancedInputLocalPlayerSubsystem* UGridActionWidget::GetEnhancedInputSubsystem() const
{
	const UWidget* BoundWidget = DisplayedBindingHandle.GetBoundWidget();
	const ULocalPlayer* BindingOwner = BoundWidget ? BoundWidget->GetOwningLocalPlayer() : GetOwningLocalPlayer();
	
	return BindingOwner->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}
