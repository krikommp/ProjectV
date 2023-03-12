// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "GameFeatures/Private/GameFeaturePluginStateMachine.h"
#include "UObject/Object.h"
#include "GridExperienceManagerComponent.generated.h"

class UGridExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridExperienceLoaded, const UGridExperienceDefinition* /*Experience*/);

enum class EGridExperienceLoadState{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

/**
 * UGridExperienceManagerComponent
 *
 * Experience 组件，用于管理 Experience 加载及获取
 */
UCLASS()
class PROJECTVGAME_API UGridExperienceManagerComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:

	UGridExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~UActorComponent interface

	// ~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~ILoadingProcessInterface interface

#if WITH_SERVER_CODE
	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
#endif

	// 该委托将会在确保 Experience 已经被加载时调用
	// 具有最高的优先级
	// 如果该 Experience 已经被加载了，那么该委托将会被立即调用
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnGridExperienceLoaded::FDelegate&& Delegate);

	// 该委托将会在确保 Experience 已经被加载时调用
	// 如果该 Experience 已经被加载了，那么该委托将会被立即调用
	void CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate&& Delegate);

	// 该委托将会在确保 Experience 已经被加载时调用
	// 如果该 Experience 已经被加载了，那么该委托将会被立即调用
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnGridExperienceLoaded::FDelegate&& Delegate);

	// 如果 Experience 被完全加载，将会返回当前使用的 Experience
	const UGridExperienceDefinition* GetCurrentExperienceChecked() const;

	// 返回是否当前 Experience 被完全加载
	bool IsExperienceLoaded() const;

private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();
private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UGridExperienceDefinition> CurrentExperience;

	EGridExperienceLoadState LoadState = EGridExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading =0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

private:
	// 当 Experience 加载完毕后调用此委托
	// 具有最高优先级，加载完毕后将会立即调用
	FOnGridExperienceLoaded OnExperienceLoad_HighPriority;

	// 当 Experience 加载完毕后调用此委托
	FOnGridExperienceLoaded OnExperienceLoaded;

	// 当 Experience 加载完毕后调用此委托
	FOnGridExperienceLoaded OnExperienceLoaded_LowPriority;
};
