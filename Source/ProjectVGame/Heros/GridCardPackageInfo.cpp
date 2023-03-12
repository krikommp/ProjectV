// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCardPackageInfo.h"

#include "GridCardInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GridLocalPlayer.h"
#include "System/GridAssetManager.h"

UGridCardPackageInfo::UGridCardPackageInfo()
{
	OwningPlayer = nullptr;
}

void UGridCardPackageInfo::SaveCardPackageFlush()
{
	check(OwningPlayer);

	UGameplayStatics::SaveGameToSlot(this, GetSaveSlotName(), OwningPlayer->GetLocalPlayerIndex());
}

UGridCardPackageInfo* UGridCardPackageInfo::CreateOrLoadCardPackage(const UGridLocalPlayer* LocalPlayer, const FGridHeroData& HeroData,
																	const FString& CardPackageID)
{
	UGridCardPackageInfo* CardPackage = nullptr;

	const FString SlotName = FString::Format(TEXT("GridPackage_{0}_{1}"), TArray<FStringFormatArg>{HeroData.HeroID, CardPackageID});
	if (LocalPlayer != nullptr)
	{
		if (UGameplayStatics::DoesSaveGameExist(SlotName, LocalPlayer->GetLocalPlayerIndex()))
		{
			USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SlotName, LocalPlayer->GetLocalPlayerIndex());
			CardPackage = Cast<UGridCardPackageInfo>(Slot);
		}
	}

	if (CardPackage == nullptr)
	{
		CardPackage = Cast<UGridCardPackageInfo>(
			UGameplayStatics::CreateSaveGameObject(UGridCardPackageInfo::StaticClass()));
		CardPackage->OwnerID = HeroData.HeroID;
		CardPackage->ID = CardPackageID;
	}

	CardPackage->Initialize(const_cast<UGridLocalPlayer*>(LocalPlayer));

	return CardPackage;
}

void UGridCardPackageInfo::Initialize(UGridLocalPlayer* LocalPlayer)
{
	if (LocalPlayer != nullptr)
	{
		OwningPlayer = LocalPlayer;
	}

	for (const auto& Pair : OwningCardIDLists)
	{
		const UGridAssetManager& AssetManager = UGridAssetManager::Get();
		const auto CardInfo = UGridCardInfo::CreateOrLoadCardInfo(OwningPlayer, Pair.Key, AssetManager.GetCardData(Pair.Value));
		AddNewCardToCardPackage(CardInfo);
	}
}

FString UGridCardPackageInfo::GetSaveSlotName() const
{
	return FString::Format(TEXT("GridPackage_{0}_{1}"), TArray<FStringFormatArg>{OwnerID, ID});
}

void UGridCardPackageInfo::AddNewCardToCardPackage(UGridCardInfo* CardToAdd)
{
	check(CardToAdd);

	OwningCardIDLists.Add(CardToAdd->CardID, CardToAdd->CardTypeID);
	OwningCardList.Add(CardToAdd);

	OnNewCardAdded.Broadcast(CardToAdd);
}

void UGridCardPackageInfo::RemoveCardFromCardPackage(UGridCardInfo* CardToRemove)
{
	check(CardToRemove);

	OwningCardIDLists.Remove(CardToRemove->CardID);
	OwningCardList.Remove(CardToRemove);

	OnRemoveCard.Broadcast();
}
