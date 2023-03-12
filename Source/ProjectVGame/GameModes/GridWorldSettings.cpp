// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldSettings.h"

#include "EngineUtils.h"
#include "GridLogChannel.h"
#include "Engine/AssetManager.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/UObjectToken.h"

AGridWorldSettings::AGridWorldSettings(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

FPrimaryAssetId AGridWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

		if (!Result.IsValid())
		{
			UE_LOG(LogGridExperience, Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
					*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}

#if WITH_EDITOR
void AGridWorldSettings::CheckForErrors()
{
	Super::CheckForErrors();

	FMessageLog MapCheck("MapCheck");

	for (TActorIterator<APlayerStart> PlayerStartIt(GetWorld()); PlayerStartIt; ++PlayerStartIt)
	{
		APlayerStart* PlayerStart = *PlayerStartIt;
		if (IsValid(PlayerStart) && PlayerStart->GetClass() == APlayerStart::StaticClass())
		{
			MapCheck.Warning()
				->AddToken(FUObjectToken::Create(PlayerStart))
				->AddToken(FTextToken::Create(FText::FromString("is a normal APlayerStart, replace with AGridPlayerStart.")));
		}
	}

	//@TODO: Make sure the soft object path is something that can actually be turned into a primary asset ID (e.g., is not pointing to an experience in an unscanned directory)
}
#endif
