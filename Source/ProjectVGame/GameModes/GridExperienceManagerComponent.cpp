// Fill out your copyright notice in the Description page of Project Settings.


#include "GridExperienceManagerComponent.h"

#include "GameFeaturesSubsystemSettings.h"
#include "GridExperienceActionSet.h"
#include "GridExperienceDefinition.h"
#include "GridExperienceManager.h"
#include "GridLogChannel.h"
#include "Net/UnrealNetwork.h"
#include "System/GridAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridExperienceManagerComponent)

namespace GridConsoleVariables
{
	static float ExperienceLoadRandomDelayMin = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayMin(
		TEXT("grid.chaos.ExperienceDelayLoad.MinSces"),
		ExperienceLoadRandomDelayMin,
		TEXT("This value (in seconds) will be added as a delay of load completion of the experience (along with the random value grid.chaos.ExperienceDelayLoad.RandomSecs)"),
		ECVF_Default);

	static float ExperienceLoadRandomDelayRange = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayRange(
		TEXT("grid.chaos.ExperienceDelayLoad.RandomSecs"),
		ExperienceLoadRandomDelayRange,
		TEXT("A random amount of time between 0 and this value (in seconds) will be added as a delay of load completion of the experience (along with the fixed value grid.chaos.ExperienceDelayLoad.MinSecs)"),
		ECVF_Default);

	float GetExperienceLoadDelayDuration()
	{
		return FMath::Max(0.0f, ExperienceLoadRandomDelayMin + FMath::FRand() * ExperienceLoadRandomDelayRange);
	}
}

UGridExperienceManagerComponent::UGridExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

#if WITH_SERVER_CODE
void UGridExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UGridAssetManager& AssetManager = UGridAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
	TSubclassOf<UGridExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass);
	const UGridExperienceDefinition* Experience = GetDefault<UGridExperienceDefinition>(AssetClass);

	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	CurrentExperience = Experience;
	StartExperienceLoad();
}
#endif

void UGridExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(FOnGridExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}else
	{
		OnExperienceLoad_HighPriority.Add(MoveTemp(Delegate));
	}
}

void UGridExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UGridExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(FOnGridExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}else
	{
		OnExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

const UGridExperienceDefinition* UGridExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EGridExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}


bool UGridExperienceManagerComponent::IsExperienceLoaded() const
{
	return (LoadState == EGridExperienceLoadState::Loaded) && (CurrentExperience != nullptr);
}

void UGridExperienceManagerComponent::OnRep_CurrentExperience()
{
	StartExperienceLoad();
}

void UGridExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience != nullptr);
	check(LoadState == EGridExperienceLoadState::Unloaded);

	UE_LOG(LogGridExperience, Log, TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s, %s)"),
		*CurrentExperience->GetPrimaryAssetId().ToString(),
		*GetClientServerContextString(this));

	LoadState = EGridExperienceLoadState::Loading;

	UGridAssetManager& AssetManager = UGridAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	for (const TObjectPtr<UGridExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// 加载 Experience 相关资产

	TArray<FName> BundlesToLoad;

	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	const TSharedPtr<FStreamableHandle> BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	const TSharedPtr<FStreamableHandle> RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartExperienceLoad()"));

	// 如果两个异步加载都是有效的 Handle, 那么就组合
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	FStreamableDelegate OnAssetLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// 如果资产已经被加载，那么就立即调用 Experience 加载完毕委托
		FStreamableHandle::ExecuteDelegate(OnAssetLoadedDelegate);
	}else
	{
		Handle->BindCompleteDelegate(OnAssetLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetLoadedDelegate]()
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			OnAssetLoadedDelegate.ExecuteIfBound();
		}));
	}

	// 这组资产将会被预加载，但是并不会影响 Experience 的加载流程，也不会 block Experience
	TSet<FPrimaryAssetId> PreloadAssetList;
	if (PreloadAssetList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(), BundlesToLoad, {});
	}
}

void UGridExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == EGridExperienceLoadState::Loading);
	check(CurrentExperience != nullptr);

	UE_LOG(LogGridExperience, Log, TEXT("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s, %s)"),
		*CurrentExperience->GetPrimaryAssetId().ToString(),
		*GetClientServerContextString(this));

	// 收集 GameFeaturePlugins 的 URLs - 过滤掉无效的映射
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This=this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	{
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
			{
				This->GameFeaturePluginURLs.AddUnique(PluginURL);
			}else
			{
				ensureMsgf(false, TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for experience %s - fix data, ignoring for this run"), *PluginName, *Context->GetPrimaryAssetId().ToString());
			}
		}
	};

	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeatureToEnable);
	for (const TObjectPtr<UGridExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			CollectGameFeaturePluginURLs(ActionSet, ActionSet->GameFeaturesToEnable);
		}
	}

	// 加载并激活 GameFeatures
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading > 0)
	{
		LoadState = EGridExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			UGridExperienceManager::NotifyOfPluginActivation(PluginURL);
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}else
	{
		OnExperienceFullLoadCompleted();
	}
}

void UGridExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// 当 GameFeature 已经被加载了，就减少记录下的正在加载的 GameFeature 计数
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void UGridExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EGridExperienceLoadState::Loaded);

	// Insert a random delay for testing (if configured)
	if (LoadState != EGridExperienceLoadState::LoadingChaosTestingDelay)
	{
		const float DelaySecs = GridConsoleVariables::GetExperienceLoadDelayDuration();
		if (DelaySecs > 0.0f)
		{
			FTimerHandle DummyHandle;

			LoadState = EGridExperienceLoadState::LoadingChaosTestingDelay;
			GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::OnExperienceFullLoadCompleted, DelaySecs, /*bLooping=*/ false);

			return;
		}
	}

	LoadState = EGridExperienceLoadState::ExecutingActions;

	// 执行 Actions
	FGameFeatureActivatingContext Context;

	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
	{
		for (UGameFeatureAction* Action : ActionList)
		{
			if (Action != nullptr)
			{
				Action->OnGameFeatureRegistering();
				Action->OnGameFeatureLoading();
				Action->OnGameFeatureActivating(Context);
			}
		}
	};

	ActivateListOfActions(CurrentExperience->Actions);
	for (const TObjectPtr<UGridExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EGridExperienceLoadState::Loaded;

	OnExperienceLoad_HighPriority.Broadcast(CurrentExperience);
	OnExperienceLoad_HighPriority.Clear();

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();

#if !UE_SERVER
	// todo...
#endif
}

void UGridExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UGridExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentExperience);
}

void UGridExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 取消所以已激活的 Experience
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		if (UGridExperienceManager::RequestToDeactivatePlugin(PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}
	}

	if (LoadState == EGridExperienceLoadState::Loaded)
	{
		LoadState = EGridExperienceLoadState::Deactivating;

		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		FGameFeatureDeactivatingContext Context(FSimpleDelegate::CreateUObject(this, &ThisClass::OnActionDeactivationCompleted));

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action)
				{
					Action->OnGameFeatureDeactivating(Context);
					Action->OnGameFeatureUnregistering();
				}
			}
		};

		DeactivateListOfActions(CurrentExperience->Actions);
		for (const TObjectPtr<UGridExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (ActionSet)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogGridExperience, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in Grid experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	} 
}

bool UGridExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != EGridExperienceLoadState::Loaded)
	{
		OutReason = TEXT("Experience still loading");
		return true;
	}
	return false;
}

void UGridExperienceManagerComponent::OnAllActionsDeactivated()
{
	LoadState = EGridExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
}











