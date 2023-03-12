// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSettingsShared.h"

#include "Kismet/GameplayStatics.h"

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");

UGridSettingsShared::UGridSettingsShared()
{
}

void UGridSettingsShared::Initialize(UGridLocalPlayer* LocalPlayer)
{
	check(LocalPlayer);

	OwningPlayer = LocalPlayer;
}

void UGridSettingsShared::SaveSettings()
{
	check(OwningPlayer);
	UGameplayStatics::SaveGameToSlot(this, SHARED_SETTINGS_SLOT_NAME, OwningPlayer->GetLocalPlayerIndex());
}


/* static */ UGridSettingsShared* UGridSettingsShared::LoadOrCreateSettings(const UGridLocalPlayer* LocalPlayer)
{
	UGridSettingsShared* SharedSettings = nullptr;

	// 如果游戏数据存在，那么就加载他
	if (UGameplayStatics::DoesSaveGameExist(SHARED_SETTINGS_SLOT_NAME, LocalPlayer->GetLocalPlayerIndex()))
	{
		USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(SHARED_SETTINGS_SLOT_NAME, LocalPlayer->GetLocalPlayerIndex());
		SharedSettings = Cast<UGridSettingsShared>(Slot);
	}

	if (SharedSettings == nullptr)
	{
		SharedSettings = Cast<UGridSettingsShared>(UGameplayStatics::CreateSaveGameObject(UGridSettingsShared::StaticClass()));
	}

	SharedSettings->Initialize(const_cast<UGridLocalPlayer*>(LocalPlayer));
	SharedSettings->ApplySettings();

	return SharedSettings;
}

void UGridSettingsShared::ApplySettings()
{
	
}

void UGridSettingsShared::ApplyInputSensitivity()
{
	
}


