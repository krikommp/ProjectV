// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "CommonActivatableWidget.h"

#include "GridActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EGridWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * UGridActivatableWidget
 *
 * An activatable widget that automatically drives the desired input config when activated
 */
UCLASS(Abstract, Blueprintable)
class PROJECTVGAME_API UGridActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UGridActivatableWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	
	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

	#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif
	
protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EGridWidgetInputMode InputConfig = EGridWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseLockMode MouseLockMode = EMouseLockMode::LockOnCapture;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	bool bShowCursor = false;
};
