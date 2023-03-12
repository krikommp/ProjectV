// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInputComponent.h"

#include "Containers/Map.h"
#include "EnhancedInputSubsystems.h"
#include "Input/GridMappableConfigPair.h"
#include "InputCoreTypes.h"
#include "Player/GridLocalPlayer.h"
#include "Settings/GridSettingsLocal.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealNames.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridInputComponent)

class UGridInputConfig;

UGridInputComponent::UGridInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UGridInputComponent::AddInputMappings(const UGridInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	UGridLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UGridLocalPlayer>();
	check(LocalPlayer);

	// Add any registered input mappings from the settings!
	if (UGridSettingsLocal* LocalSettings = UGridSettingsLocal::Get())
	{	
		// Tell enhanced input about any custom keymappings that the player may have customized
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
			}
		}
	}
}

void UGridInputComponent::RemoveInputMappings(const UGridInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	UGridLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UGridLocalPlayer>();
	check(LocalPlayer);
	
	if (UGridSettingsLocal* LocalSettings = UGridSettingsLocal::Get())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}
		
		// Clear any player mapped keys from enhanced input
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			InputSubsystem->RemovePlayerMappedKey(Pair.Key);
		}
	}
}

void UGridInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
