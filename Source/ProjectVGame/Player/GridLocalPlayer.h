// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AudioMixerBlueprintLibrary.h"
#include "CommonLocalPlayer.h"
#include "Containers/UnrealString.h"
#include "GenericTeamAgentInterface.h"
#include "HAL/Platform.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "GridLocalPlayer.generated.h"

class APlayerController;
class UInputMappingContext;
class UObject;
class UWorld;
struct FFrame;
struct FSwapAudioOutputResult;
class UGridSettingsShared;
class UGridSettingsLocal;
class UGridHeroInfo;

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()
public:
	UGridLocalPlayer();

public:
	UFUNCTION()
	UGridSettingsLocal* GetLocalSettings() const;
	
	UFUNCTION()
	UGridSettingsShared* GetSharedSettings() const;

	UFUNCTION()
	UGridHeroInfo* GetHeroInfo(const FName& HeroId) const;
private:
	
	// SharedSettings, 使用 mutable 修饰，表示其可以在 const 函数中被修改
	UPROPERTY(Transient)
	mutable TObjectPtr<UGridSettingsShared> SharedSettings;

	UPROPERTY(Transient)
	mutable TMap<FName, TObjectPtr<UGridHeroInfo>> HeroInfoMap;
};
