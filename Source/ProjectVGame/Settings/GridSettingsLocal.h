// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonInputBaseTypes.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "GameFramework/GameUserSettings.h"
#include "Input/GridMappableConfigPair.h"
#include "InputCoreTypes.h"
#include "Scalability.h"
#include "Input/GridMappableConfigPair.h"
#include "UObject/NameTypes.h"
#include "UObject/UObjectGlobals.h"

#include "GridSettingsLocal.generated.h"

class UGridLocalPlayer;

/**
 * UGridSettingsLocal
 *
 * 玩家设置
 */
UCLASS()
class PROJECTVGAME_API UGridSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()

public:

	UGridSettingsLocal();

	static UGridSettingsLocal* Get();
public:

	DECLARE_EVENT_OneParam(UGridSettingsLocal, FInputConfigDelegate, const FLoadedMappableConfigPair& /*Config*/);

	// 当新的输入配置被注册时调用
	FInputConfigDelegate OnInputConfigRegistered;

	// 当有输入配置被激活时调用
	FInputConfigDelegate OnInputConfigActivated;

	// 当有输入配置被注销时调用
	FInputConfigDelegate OnInputConfigDeactivated;

	// 注册输入配置，并且激活
	void RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, const bool bIsActive);

	// 注销输入配置，并且返回移除的数量
	int32 UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove);

	// 通过名称获取输入配置，如果不存在则返回空值
	UFUNCTION(BlueprintCallable)
	const UPlayerMappableInputConfig* GetInputConfigByName(FName ConfigName) const;

	// 获取当前所有注册的输入配置
	const TArray<FLoadedMappableConfigPair>& GetAllRegisteredInputConfigs() const { return RegisteredInputConfigs; }
	
	// 根据输入类型获取输入配置
	void GetRegisteredInputConfigsOfType(ECommonInputType Type, OUT TArray<FLoadedMappableConfigPair>& OutArray) const;

	// 根据 key 值获取所有绑定的输入行为
	void GetAllMappingNamesFromKey(const FKey InKey, TArray<FName>& OutActionNames);

	// 将给定的按键绑定到一个新值
	void AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, UGridLocalPlayer* LocalPlayer);

	// 将按键恢复到默认绑定
	void ResetKeybindingToDefault(const FName MappingName, UGridLocalPlayer* LocalPlayer);

	// 将所有按键都恢复到默认绑定
	void ResetKeybindingsToDefault(UGridLocalPlayer* LocalPlayer);
	
	const TMap<FName, FKey>& GetCustomPlayerInputConfig() const { return CustomKeyboardConfig; }
	
private:

	// 玩家当前选中的输入配置
	// 用于在重定向输入时使用
	UPROPERTY(Config)
	FName InputConfigName = TEXT("Default");
	
	// 记录当前已经注册的输入配置数组
	UPROPERTY(VisibleAnywhere)
	TArray<FLoadedMappableConfigPair> RegisteredInputConfigs;

	// 玩家自定义按键绑定，默认为空
	UPROPERTY(Config)
	TMap<FName, FKey> CustomKeyboardConfig;

private:
	void OnAppActivationStateChanged(bool bIsActive); 

private:
	FDelegateHandle OnApplicationActivationStateChangedHandle;
};
