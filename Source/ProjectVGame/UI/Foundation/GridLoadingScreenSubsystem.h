// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Delegates/Delegate.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
#include "UObject/UObjectGlobals.h"

#include "GridLoadingScreenSubsystem.generated.h"

class UObject;
class UUserWidget;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenWidgetChangedDelegate, TSubclassOf<UUserWidget>, NewWidgetClass);

/**
 * UGridLoadingScreenSubsystem
 *
 * 保存当前的 loading screen 配置
 */
UCLASS()
class PROJECTVGAME_API UGridLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGridLoadingScreenSubsystem();

	// Sets the loading screen widget class to display inside of the loading screen widget host
	UFUNCTION(BlueprintCallable)
	void SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	// Returns the last set loading screen widget class to display inside of the loading screen widget host
	UFUNCTION(BlueprintPure)
	TSubclassOf<UUserWidget> GetLoadingScreenContentWidget() const;

private:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess))
	FLoadingScreenWidgetChangedDelegate OnLoadingScreenWidgetChanged;

	UPROPERTY()
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
};
