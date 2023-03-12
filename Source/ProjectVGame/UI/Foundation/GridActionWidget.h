// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActionWidget.h"
#include "UObject/Object.h"
#include "GridActionWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;

/**
 * UGridActionWidget
 *
 * 用于显示输入操作的图标
 * 例如，希望显示手柄对应按键图标
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTVGAME_API UGridActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()

public:

	// ~UCommonActionWidget interface
	virtual FSlateBrush GetIcon() const override;
	// ~UCommonActionWidget interface
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	const TObjectPtr<UInputAction> AssociatedInputAction;

private:
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
