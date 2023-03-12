// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GridActivatableWidget.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "GridHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;

/**
 * UGridHUDLayout
 *
 * Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Grid HUD Layout", Category = "Grid|HUD"))
class PROJECTVGAME_API UGridHUDLayout : public UGridActivatableWidget
{
	GENERATED_BODY()

public:

	UGridHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
