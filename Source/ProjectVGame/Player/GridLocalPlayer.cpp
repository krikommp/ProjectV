// Fill out your copyright notice in the Description page of Project Settings.


#include "GridLocalPlayer.h"

#include "Heros/GridHeroInfo.h"
#include "Settings/GridSettingsLocal.h"
#include "Settings/GridSettingsShared.h"
#include "System/GridAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridLocalPlayer)

UGridLocalPlayer::UGridLocalPlayer()
{
}

UGridSettingsLocal* UGridLocalPlayer::GetLocalSettings() const
{
	return UGridSettingsLocal::Get();
}

UGridSettingsShared* UGridLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		SharedSettings = UGridSettingsShared::LoadOrCreateSettings(this);
	}

	return SharedSettings;
}

UGridHeroInfo* UGridLocalPlayer::GetHeroInfo(const FName& HeroId) const
{
	auto HeroInfo = HeroInfoMap.FindRef(HeroId);
	if (!HeroInfo)
	{
		const UGridAssetManager& AssetManager = UGridAssetManager::Get();
		const auto HeroData = AssetManager.GetHeroData(HeroId);
		HeroInfo = UGridHeroInfo::LoadOrCreateHeroInfo(HeroData, this);
		HeroInfoMap.Add(HeroId, HeroInfo);
	}
	return HeroInfo;
}


