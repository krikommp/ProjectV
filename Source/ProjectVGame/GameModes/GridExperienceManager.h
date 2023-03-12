// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridExperienceManager.generated.h"

/**
 * Experience 管理器
 */
UCLASS(MinimalAPI)
class UGridExperienceManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	PROJECTVGAME_API void OnPlayInEditorBegun();

	static void NotifyOfPluginActivation(const FString PluginURL);
	static bool RequestToDeactivatePlugin(const FString PluginURL);
#else
	static void NotifyOfPluginActivation(const FString PluginURL) {}
	static bool RequestToDeactivatePlugin(const FString PluginURL) { return true; }
#endif

private:

	// 记录 GameFeature Plugin 的请求次数映射
	TMap<FString, int32> GameFeaturePluginRequestCountMap;
};
