// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSettingsLocal.h"

#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Player/GridLocalPlayer.h"

UGridSettingsLocal::UGridSettingsLocal()
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && FSlateApplication::IsInitialized())
	{
		OnApplicationActivationStateChangedHandle = FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &ThisClass::OnAppActivationStateChanged);
	}
}

UGridSettingsLocal* UGridSettingsLocal::Get()
{
	return GEngine ? CastChecked<UGridSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UGridSettingsLocal::RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig,
	const bool bIsActive)
{
	if (NewConfig)
	{
		const int32 ExistingConfigIdx = RegisteredInputConfigs.IndexOfByPredicate( [&NewConfig](const FLoadedMappableConfigPair& Pair) { return Pair.Config == NewConfig; } );
		if (ExistingConfigIdx == INDEX_NONE)
		{
			const int32 NumAdded = RegisteredInputConfigs.Add(FLoadedMappableConfigPair(NewConfig, Type, bIsActive));
			if (NumAdded != INDEX_NONE)
			{
				OnInputConfigRegistered.Broadcast(RegisteredInputConfigs[NumAdded]);
			}	
		}
	}
}

int32 UGridSettingsLocal::UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove)
{
	if (ConfigToRemove)
	{
		const int32 Index = RegisteredInputConfigs.IndexOfByPredicate( [&ConfigToRemove](const FLoadedMappableConfigPair& Pair) { return Pair.Config == ConfigToRemove; } );
		if (Index != INDEX_NONE)
		{
			RegisteredInputConfigs.RemoveAt(Index);
			return 1;
		}
			
	}
	return INDEX_NONE;
}

const UPlayerMappableInputConfig* UGridSettingsLocal::GetInputConfigByName(FName ConfigName) const
{
	for (const FLoadedMappableConfigPair& Pair : RegisteredInputConfigs)
	{
		if (Pair.Config->GetConfigName() == ConfigName)
		{
			return Pair.Config;
		}
	}
	return nullptr;
}

void UGridSettingsLocal::GetRegisteredInputConfigsOfType(ECommonInputType Type,
	TArray<FLoadedMappableConfigPair>& OutArray) const
{
	OutArray.Empty();

	// If "Count" is passed in then 
	if (Type == ECommonInputType::Count)
	{
		OutArray = RegisteredInputConfigs;
		return;
	}
	
	for (const FLoadedMappableConfigPair& Pair : RegisteredInputConfigs)
	{
		if (Pair.Type == Type)
		{
			OutArray.Emplace(Pair);
		}
	}
}

void UGridSettingsLocal::GetAllMappingNamesFromKey(const FKey InKey, TArray<FName>& OutActionNames)
{
	if (InKey == EKeys::Invalid)
	{
		return;
	}

	// adding any names of actions that are bound to that key
	for (const FLoadedMappableConfigPair& Pair : RegisteredInputConfigs)
	{
		if (Pair.Type == ECommonInputType::MouseAndKeyboard)
		{
			for (const FEnhancedActionKeyMapping& Mapping : Pair.Config->GetPlayerMappableKeys())
			{
				FName MappingName(Mapping.PlayerMappableOptions.DisplayName.ToString());
				FName ActionName = Mapping.PlayerMappableOptions.Name;
				// make sure it isn't custom bound as well
				if (const FKey* MappingKey = CustomKeyboardConfig.Find(ActionName))
				{
					if (*MappingKey == InKey)
					{
						OutActionNames.Add(MappingName);
					}
				}
				else
				{
					if (Mapping.Key == InKey)
					{
						OutActionNames.Add(MappingName);
					}
				}
			}
		}
	}
}

void UGridSettingsLocal::AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey,
	UGridLocalPlayer* LocalPlayer)
{
	if (MappingName == NAME_None)
	{
		return;
	}
	
	if (InputConfigName != TEXT("Custom"))
	{
		// Copy Presets.
		if (const UPlayerMappableInputConfig* DefaultConfig = GetInputConfigByName(TEXT("Default")))
		{
			for (const FEnhancedActionKeyMapping& Mapping : DefaultConfig->GetPlayerMappableKeys())
			{
				// Make sure that the mapping has a valid name, its possible to have an empty name
				// if someone has marked a mapping as "Player Mappable" but deleted the default field value
				if (Mapping.PlayerMappableOptions.Name != NAME_None)
				{
					CustomKeyboardConfig.Add(Mapping.PlayerMappableOptions.Name, Mapping.Key);
				}
			}
		}
		
		InputConfigName = TEXT("Custom");
	} 

	if (FKey* ExistingMapping = CustomKeyboardConfig.Find(MappingName))
	{
		// Change the key to the new one
		CustomKeyboardConfig[MappingName] = NewKey;
	}
	else
	{
		CustomKeyboardConfig.Add(MappingName, NewKey);
	}

	// Tell the enhanced input subsystem for this local player that we should remap some input! Woo
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->AddPlayerMappedKey(MappingName, NewKey);
	}
}

void UGridSettingsLocal::ResetKeybindingToDefault(const FName MappingName, UGridLocalPlayer* LocalPlayer)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->RemovePlayerMappedKey(MappingName);
	}
}

void UGridSettingsLocal::ResetKeybindingsToDefault(UGridLocalPlayer* LocalPlayer)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->RemoveAllPlayerMappedKeys();
	}
}

void UGridSettingsLocal::OnAppActivationStateChanged(bool bIsActive)
{
}
