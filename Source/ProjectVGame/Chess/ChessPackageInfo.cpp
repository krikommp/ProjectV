// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPackageInfo.h"

#include "Kismet/GameplayStatics.h"
#include "Player/GridLocalPlayer.h"
#include "Settings/GridSettingDefaultValues.h"

UChessPackageInfo::UChessPackageInfo(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UChessPackageInfo* UChessPackageInfo::LoadOrCreateChessPackageInfo(const FString& PlayerName,
	const UGridLocalPlayer* LocalPlayer)
{
	UChessPackageInfo* NewChessPackageInfo = nullptr;
	const FString SlotName = FString::Format(TEXT("ChessPackageInfo_{0}"), {PlayerName});

	if (UGameplayStatics::DoesSaveGameExist(SlotName, LocalPlayer->GetLocalPlayerIndex()))
	{
		USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SlotName, LocalPlayer->GetLocalPlayerIndex());
		NewChessPackageInfo = Cast<UChessPackageInfo>(Slot);
	}

	if (NewChessPackageInfo == nullptr)
	{
		NewChessPackageInfo = Cast<UChessPackageInfo>(UGameplayStatics::CreateSaveGameObject(UChessPackageInfo::StaticClass()));
		NewChessPackageInfo->ChessIDList = GetMutableDefault<UGridSettingDefaultValues>()->DefaultChessIDs;
		NewChessPackageInfo->SlotName = SlotName;
	}

	ensureAlwaysMsgf(NewChessPackageInfo, TEXT("Can not create UChessPackageInfo, crash!!"));

	NewChessPackageInfo->Initialize(const_cast<UGridLocalPlayer*>(LocalPlayer));
	
	return NewChessPackageInfo;
}

void UChessPackageInfo::Initialize(UGridLocalPlayer* InLocalPlayer)
{
	check(InLocalPlayer);

	OwningPlayer = InLocalPlayer;
}
