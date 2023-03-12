// Fill out your copyright notice in the Description page of Project Settings.


#include "GridUICardManagerSubsystem.h"

#include "GridLogChannel.h"
#include "System/GridAssetManager.h"
#include "UI/Card/GridCard.h"

UGridUICardManagerSubsystem::UGridUICardManagerSubsystem()
{
}

void UGridUICardManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UGridAssetManager& AssetManager = UGridAssetManager::Get();
	ConfigDataAsset = AssetManager.GetAsset(ConfigDataAsset);

	CardWidgetPool = MakeShared<FWidgetObjectPool<UGridCard>>(GetWorld(), DefaultCardNumInPool, CardDefaultClass);
}

void UGridUICardManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UGridCard* UGridUICardManagerSubsystem::GetCardWidget() const
{
	return CardWidgetPool->GetWidget();
}

void UGridUICardManagerSubsystem::ReturnCardWidget(UGridCard* ReturnCard) const
{
	ReturnCard->RecycleCard();
	CardWidgetPool->ReturnWidget(ReturnCard);
}

const UGridPlayerHandConfigDataAsset* UGridUICardManagerSubsystem::GetPlayerHandConfig() const
{
	return ConfigDataAsset.Get();
}
