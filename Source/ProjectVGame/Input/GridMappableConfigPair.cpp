// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMappableConfigPair.h"

#include "CommonUISettings.h"
#include "ICommonUIModule.h"
#include "PlayerMappableInputConfig.h"
//#include "Settings/GridSettingsLocal.h"
#include "Settings/GridSettingsLocal.h"
#include "System/GridAssetManager.h"
#include "Templates/Casts.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridMappableConfigPair)

bool FMappableConfigPair::CanBeActivated() const
{
	const FGameplayTagContainer& PlatformTraits = ICommonUIModule::GetSettings().GetPlatformTraits();

	// If the current platform does NOT have all the dependent traits, then don't activate it
	if (!DependentPlatformTraits.IsEmpty() && !PlatformTraits.HasAll(DependentPlatformTraits))
	{
		return false;
	}

	// If the platform has any of the excluded traits, then we shouldn't activate this config.
	if (!ExcludedPlatformTraits.IsEmpty() && PlatformTraits.HasAny(ExcludedPlatformTraits))
	{
		return false;
	}

	return true;
}

bool FMappableConfigPair::RegisterPair(const FMappableConfigPair& Pair)
{
	UGridAssetManager& AssetManager = UGridAssetManager::Get();

	if (UGridSettingsLocal* Settings = UGridSettingsLocal::Get())
	{
		// Register the pair with the settings, but do not activate it yet
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->RegisterInputConfig(Pair.Type, LoadedConfig, false);
			return true;
		}	
	}
	
	return false;
}

void FMappableConfigPair::UnregisterPair(const FMappableConfigPair& Pair)
{
	UGridAssetManager& AssetManager = UGridAssetManager::Get();

	if (UGridSettingsLocal* Settings = UGridSettingsLocal::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.Config))
		{
			Settings->UnregisterInputConfig(LoadedConfig);
		}
	}
}