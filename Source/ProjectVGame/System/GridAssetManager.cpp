// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "AbilitySystem/GridGameplayCueManager.h"
#include "Character/GridPawnData.h"
#include "Misc/ScopedSlowTask.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("Grid.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UGridAssetManager::DumpLoadedAssets)
	);

////////////////////////////////////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FGridAssetManagerStartupJob(#JobFunc, [this](const FGridAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

////////////////////////////////////////////////////////////////////////////////////////////////////

UGridAssetManager::UGridAssetManager()
{
	DefaultPawnData = nullptr;
}

UGridAssetManager& UGridAssetManager::Get()
{
	check(GEngine);

	if (UGridAssetManager* Singleton = Cast<UGridAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogGrid, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to GridAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UGridAssetManager>();
}

UObject* UGridAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UGridAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UGridAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UGridAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogGrid, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogGrid, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogGrid, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogGrid, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

void UGridAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UGridAssetManager::StartInitialLoading");

	// 这里会进行资产扫描，所以我们等待他完全，之后再进行资产加载
	Super::StartInitialLoading();

	STARTUP_JOB(InitializeAbilitySystem());
	STARTUP_JOB(InitializeGameplayCueManager());

	{
		// 加载游戏数据资产
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
		// 加载英雄资产
		STARTUP_JOB_WEIGHTED(LoadHeroData(), 25.f);
		// 加载卡牌资产
		STARTUP_JOB_WEIGHTED(LoadCardData(), 25.f);
		// 加载技能资产
		STARTUP_JOB_WEIGHTED(LoadSkillData(), 25.f);
	}

	// 开始执行开始任务
	DoAllStartupJobs();
}

void UGridAssetManager::InitializeAbilitySystem()
{
	SCOPED_BOOT_TIMING("UGridAssetManager::InitializeAbilitySystem");

	FGridGameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

void UGridAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("UGridAssetManager::InitializeGameplayCueManager");

	UGridGameplayCueManager* GCM = UGridGameplayCueManager::Get();
	check(GCM);
	GCM->LoadAlwaysLoadedCues();
}

const UGridGameData& UGridAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UGridGameData>(GridGameDataPath);
}

const UGridPawnData* UGridAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

const FGridHeroData& UGridAssetManager::GetHeroData(int32 HeroID) const
{
	return HeroDataMap.FindChecked(HeroID);
}

bool UGridAssetManager::CheckCardData(const FString& CardId) const
{
	return CardDataMap.Contains(CardId);
}

const FGridCardData& UGridAssetManager::GetCardData(const FString& CardId) const
{
	return CardDataMap.FindChecked(CardId);
}

const FGridSkillData& UGridAssetManager::GetSkillData(const FName& SkillId) const
{
	return SkillDataMap.FindChecked(SkillId);
}

void UGridAssetManager::LoadHeroData()
{
	const auto DataTable = GetAsset(HeroDataTable);
	if (DataTable == nullptr)
	{
		UE_LOG(LogGrid, Error, TEXT("Failed to Load Hero Data!"));
		return;
	}

	static const FString HeroContext = FString("UGridAssetManager::LoadHeroData");
	TArray<FGridHeroData*> HeroDataList;
	DataTable->GetAllRows<FGridHeroData>(HeroContext, HeroDataList);
	for (const auto& HeroData : HeroDataList)
	{
		HeroDataMap.Add(HeroData->HeroID, *HeroData);
	}
}

void UGridAssetManager::LoadCardData()
{
	const auto DataTable = GetAsset(CardDataTable);
	if (DataTable == nullptr)
	{
		UE_LOG(LogGrid, Error, TEXT("Failed to Load Hero Data!"));
		return;
	}

	static const FString CardContext = FString("UGridAssetManager::LoadCardData");
	TArray<FGridCardData*> CardDataList;
	DataTable->GetAllRows<FGridCardData>(CardContext, CardDataList);
	for (const auto& CardData : CardDataList)
	{
		CardDataMap.Add(CardData->CardID, *CardData);
	}
}

void UGridAssetManager::LoadSkillData()
{
	const auto DataTable = GetAsset(SkillDataTable);
	if (DataTable == nullptr)
	{
		UE_LOG(LogGrid, Error, TEXT("Failed to Load Hero Data!"));
		return;
	}

	static const FString SkillContext = FString("UGridAssetManager::LoadSkillData");
	TArray<FGridSkillData*> SkillDataList;
	DataTable->GetAllRows<FGridSkillData>(SkillContext, SkillDataList);
	for (const auto& SkillData : SkillDataList)
	{
		SkillDataMap.Add(SkillData->ID, *SkillData);
	}
}

UPrimaryDataAsset* UGridAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("GridEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogGrid, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogGrid, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}

void UGridAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("UGridAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		for (const FGridAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.0f;
			for (const FGridAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.0f;
			for (FGridAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}

	StartupJobs.Empty();

	UE_LOG(LogGrid, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UGridAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	
}

#if WITH_EDITOR
void UGridAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("GridEditor", "BeginLoadingPIEData", "Loading PIE Data"));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		const UGridGameData& LocalGameDataCommon = GetGameData();

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}

#endif



