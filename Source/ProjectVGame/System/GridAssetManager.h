// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridAssetManagerStartupJob.h"
#include "Engine/AssetManager.h"
#include "Engine/DataAsset.h"
#include "GridGameData.h"
#include "Heros/GridCardData.h"
#include "Heros/GridHeroData.h"
#include "Heros/GridSkillData.h"
#include "GridAssetManager.generated.h"

class UGridGameData;
class UGridPawnData;
class UDataTable;

/**
 * UGridAssetManager
 *
 * 游戏资源加载管理类
 * 这个类在 DefaultEngine.ini 配置文件中，可以通过 'AssetManagerClassName' 项来设置 
 */
UCLASS(Config = Game)
class PROJECTVGAME_API UGridAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	UGridAssetManager();

	// 返回 GridAssetManager 单例对象
	static UGridAssetManager& Get();

	// 通过 TSoftObjectPtr 来获取资源对象. 如果资源没有加载到内存中，将会执行同步加载
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// 通过 TSoftClassPtr 来获取资源的 SubClass. 如果资源没有加载到内存中，将会执行同步加载
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// 打印当前已经被资源管理器加载的所有资源对象
	static void DumpLoadedAssets();

	const UGridGameData& GetGameData();
	const UGridPawnData* GetDefaultPawnData() const;
	const FGridHeroData& GetHeroData(const FName& HeroID) const;
	bool CheckCardData(const FString& CardId) const;
	const FGridCardData& GetCardData(const FString& CardId) const;
	const FGridSkillData& GetSkillData(const FName& SkillId) const;

	// 编辑器方法
#if WITH_EDITOR
	UFUNCTION(meta=(MenuItem="FeelingSoul/Reload Game Data", ToolTip="Reloads all game data from disk"))
	static void ReloadGameData();
#endif

protected:

	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const * pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();

	// 加载英雄数据表流程
	void LoadHeroData();

	// 加载卡牌数据
	void LoadCardData();

	// 加载技能数据
	void LoadSkillData();

	// 将已加载的资源对象加入到全局资源缓存中
	// 线程安全
	void AddLoadedAsset(const UObject* Asset);

	//~UAssetManager interface
	virtual void StartInitialLoading() override;
#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~End of UAssetManager interface

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

protected:

	// 全局游戏对象
	// 该值需要在配置文件中指定
	UPROPERTY(Config)
	TSoftObjectPtr<UGridGameData> GridGameDataPath;

	// 已加载入内存中的游戏资源缓存
	// 临时对象，不会被序列化保存
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

	// 如果在 PlayerState 中没有设置任何 PawnData， 那么就使用默认 PawnData
	UPROPERTY(Config)
	TSoftObjectPtr<UGridPawnData> DefaultPawnData;

	// 全局英雄数据
	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> HeroDataTable;

	// 已加载入内存的英雄数据
	UPROPERTY(Transient)
	TMap<FName, FGridHeroData> HeroDataMap;

	// 全局卡牌数据
	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> CardDataTable;

	// 全局技能数据
	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> SkillDataTable;

	// 已加载入内存的卡牌数据
	UPROPERTY(Transient)
	TMap<FString, FGridCardData> CardDataMap;

	// 已加载入内存的技能数据
	UPROPERTY(Transient)
	TMap<FName, FGridSkillData> SkillDataMap;
private:

	// 刷新所有 StartupJobs 线程数组。开始启动工作
	void DoAllStartupJobs();

	// 初始化 AbilitySystem
	void InitializeAbilitySystem();
	void InitializeGameplayCueManager();

	// 将会在加载过程中被调用，用于向屏幕提供有关加载信息
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	// 启动时需要执行的任务列表，用于提供进度消息
	TArray<FGridAssetManagerStartupJob> StartupJobs;

private:

	// 所有需要被追踪的已加载资产缓存
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection LoadedAssetsCritical;
};

template<typename AssetType>
AssetType* UGridAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename AssetType>
TSubclassOf<AssetType> UGridAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
