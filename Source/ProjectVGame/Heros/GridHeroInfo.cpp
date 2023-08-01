// Fill out your copyright notice in the Description page of Project Settings.


#include "GridHeroInfo.h"

#include "GridCardInfo.h"
#include "GridCardPackageInfo.h"
#include "GridSkillInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GridLocalPlayer.h"
#include "System/GridAssetManager.h"


UGridHeroInfo::UGridHeroInfo()
{
	HeroID = FName();
	HeroLevel = INDEX_NONE;
	HoldingCardPackageIDs.Empty();
}

void UGridHeroInfo::SaveHeroInfoFlush()
{
	check(OwningPlayer);
	UGameplayStatics::SaveGameToSlot(this, GetSaveSlotName(), OwningPlayer->GetLocalPlayerIndex());
}

UGridHeroInfo* UGridHeroInfo::LoadOrCreateHeroInfo(const FGridHeroData& HeroData, const UGridLocalPlayer* LocalPlayer)
{
	UGridHeroInfo* HeroInfo = nullptr;
	const FString SlotName = FString::Format(TEXT("GridHeroInfo_{0}"), TArray<FStringFormatArg>({ HeroData.HeroID.ToString() }));

	bool bDefault = true;
	if (UGameplayStatics::DoesSaveGameExist(SlotName,LocalPlayer->GetLocalPlayerIndex()))
	{
		USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SlotName, LocalPlayer->GetLocalPlayerIndex());
		HeroInfo = Cast<UGridHeroInfo>(Slot);
		bDefault = false;
	}

	if (HeroInfo == nullptr)
	{
		HeroInfo = Cast<UGridHeroInfo>(UGameplayStatics::CreateSaveGameObject(UGridHeroInfo::StaticClass()));
		HeroInfo->HeroID = HeroData.HeroID;
		HeroInfo->HeroLevel = HeroData.DefaultLevel;
		HeroInfo->UsingCardPackageID = FGuid::NewGuid().ToString();
		HeroInfo->HoldingCardPackageIDs.Add(HeroInfo->UsingCardPackageID);
	}

	ensureAlwaysMsgf(HeroInfo, TEXT("Can not create UGridHeroInfo, crash!!"));

	HeroInfo->HeroData = HeroData;
	HeroInfo->Initialize(const_cast<UGridLocalPlayer*>(LocalPlayer), bDefault);

	return HeroInfo;
}

UGridHeroInfo* UGridHeroInfo::CreateHeroInfo(const FGridHeroData& HeroData)
{
	UGridHeroInfo* HeroInfo = Cast<UGridHeroInfo>(UGameplayStatics::CreateSaveGameObject(UGridHeroInfo::StaticClass()));
	HeroInfo->HeroID = HeroData.HeroID;
	HeroInfo->HeroLevel = HeroData.DefaultLevel;
	HeroInfo->UsingCardPackageID = FGuid::NewGuid().ToString();
	HeroInfo->HoldingCardPackageIDs.Add(HeroInfo->UsingCardPackageID);
	HeroInfo->SkillList = HeroData.DefaultSkillList;
	
	ensureAlwaysMsgf(HeroInfo, TEXT("Can not create UGridHeroInfo, crash!!"));

	HeroInfo->HeroData = HeroData;
	HeroInfo->Initialize(nullptr, true);
	
	return HeroInfo;
}

void UGridHeroInfo::Initialize(UGridLocalPlayer* InLocalPlayer, bool bDefault)
{
	if (InLocalPlayer != nullptr)
	{
		OwningPlayer = InLocalPlayer;
	}

	for (const FString& CardPackageId : HoldingCardPackageIDs)
	{
		auto CardPackage = UGridCardPackageInfo::CreateOrLoadCardPackage(OwningPlayer, HeroData, CardPackageId);

		if (bDefault && CardPackage->OwningCardList.IsEmpty())
		{
			for (const auto& CardID : HeroData.DefaultCardIDs)
			{
				const UGridAssetManager& AssetManager = UGridAssetManager::Get();
				const FGridCardData& CardData = AssetManager.GetCardData(CardID);
				UGridCardInfo* CardInfo = UGridCardInfo::CreateOrLoadCardInfo(OwningPlayer, FGuid::NewGuid().ToString(), CardData);
				CardPackage->AddNewCardToCardPackage(CardInfo);
			}
			CardPackage->DisplayName = HeroData.DefaultCardPackageName;
		}
		
		HoldingCardPackages.Add(CardPackageId, CardPackage);
	}

	for (const FName& SkillId : SkillList)
	{
		const UGridAssetManager& AssetManager = UGridAssetManager::Get();
		const FGridSkillData& SkillData = AssetManager.GetSkillData(SkillId);
		UGridSkillInfo* SkillInfo = UGridSkillInfo::CreateOrLoadSkillInfo(OwningPlayer, HeroID, SkillData);
		SkillInfoMap.Add({ SkillId, SkillInfo });
	}

	RuntimeFriendlyFactions.AddTag(HeroData.Faction);
	// for (const auto& Faction : HeroData.FriendlyFactions)
	// {
	// 	RuntimeFriendlyFactions.AddTag(Faction);
	// }
	RuntimeFaction = HeroData.Faction;
}

FString UGridHeroInfo::GetSaveSlotName() const
{
	return FString::Format(TEXT("GridHeroInfo_{0}"), TArray<FStringFormatArg>({ HeroID.ToString() }));
}
