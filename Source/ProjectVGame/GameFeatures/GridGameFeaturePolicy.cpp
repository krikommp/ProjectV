// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameFeaturePolicy.h"

UGridGameFeaturePolicy& UGridGameFeaturePolicy::Get()
{
	return UGameFeaturesSubsystem::Get().GetPolicy<UGridGameFeaturePolicy>();
}

UGridGameFeaturePolicy::UGridGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGridGameFeaturePolicy::InitGameFeatureManager()
{
	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	Super::InitGameFeatureManager();
}

void UGridGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}
	Observers.Empty();
}

TArray<FPrimaryAssetId> UGridGameFeaturePolicy::GetPreloadAssetListForGameFeature(
	const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets) const
{
	return Super::GetPreloadAssetListForGameFeature(GameFeatureToLoad, bIncludeLoadedAssets);
}

bool UGridGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const
{
	return Super::IsPluginAllowed(PluginURL);
}

const TArray<FName> UGridGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const
{
	return Super::GetPreloadBundleStateForGameFeature();
}

void UGridGameFeaturePolicy::GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const
{
	// Editor will load both, this can cause hitching as the bundles are set to not preload in editor
	bLoadClientData = !IsRunningDedicatedServer();
	bLoadServerData = !IsRunningClientOnly();
}
