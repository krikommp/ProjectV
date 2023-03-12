// Fill out your copyright notice in the Description page of Project Settings.


#include "GridUserFacingExperienceDefinition.h"

#include "CommonSessionSubsystem.h"

UCommonSession_HostSessionRequest* UGridUserFacingExperienceDefinition::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();
	const FString UserFacingExperienceName = GetPrimaryAssetId().PrimaryAssetName.ToString();
	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->OnlineMode = ECommonSessionOnlineMode::Online;
	Result->bUseLobbies = true;
	Result->MapID = MapID;
	Result->ModeNameForAdvertisement = UserFacingExperienceName;
	Result->ExtraArgs = ExtraArgs;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);
	Result->MaxPlayerCount = MaxPlayerCount;

	return Result;
}
