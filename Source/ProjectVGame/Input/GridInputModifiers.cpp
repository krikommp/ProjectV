// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInputModifiers.h"

#include <Settings/GridSettingsShared.h>

#include "EnhancedPlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Player/GridLocalPlayer.h"

DEFINE_LOG_CATEGORY_STATIC(LogGridInputModifiers, Log, All);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GridInputModifiersHelpers
namespace GridInputModifiersHelpers
{
	static UGridLocalPlayer* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		if (PlayerInput)
		{
			if (APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
			{
				return Cast<UGridLocalPlayer>(PC->GetLocalPlayer());
			}
		}
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UGridSettingBasedScalar

FInputActionValue UGridSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                    FInputActionValue CurrentValue, float DeltaTime)
{
	if (ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean,
	               TEXT("Setting Based Scalar modifier doesn't support boolean values.")))
	{
		if (UGridLocalPlayer* LocalPlayer = GridInputModifiersHelpers::GetLocalPlayer(PlayerInput))
		{
			const UClass* SettingsClass = UGridSettingsShared::StaticClass();
			UGridSettingsShared* SharedSettings = LocalPlayer->GetSharedSettings();

			const bool bHasCachedProperty = PropertyCache.Num() == 3;

			const FProperty* XAxisValue = bHasCachedProperty
				                              ? PropertyCache[0]
				                              : SettingsClass->FindPropertyByName(XAxisScalarSettingName);
			const FProperty* YAxisValue = bHasCachedProperty
				                              ? PropertyCache[1]
				                              : SettingsClass->FindPropertyByName(YAxisScalarSettingName);
			const FProperty* ZAxisValue = bHasCachedProperty
				                              ? PropertyCache[2]
				                              : SettingsClass->FindPropertyByName(ZAxisScalarSettingName);

			if (PropertyCache.IsEmpty())
			{
				PropertyCache.Emplace(XAxisValue);
				PropertyCache.Emplace(YAxisValue);
				PropertyCache.Emplace(ZAxisValue);
			}

			FVector ScalarToUse = FVector(1.0, 1.0, 1.0);

			switch (CurrentValue.GetValueType())
			{
			case EInputActionValueType::Axis3D:
				ScalarToUse.Z = ZAxisValue ? *ZAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
			//[[fallthrough]];
			case EInputActionValueType::Axis2D:
				ScalarToUse.Y = YAxisValue ? *YAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
			//[[fallthrough]];
			case EInputActionValueType::Axis1D:
				ScalarToUse.X = XAxisValue ? *XAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				break;
			}

			ScalarToUse.X = FMath::Clamp(ScalarToUse.X, MinValueClamp.X, MaxValueClamp.X);
			ScalarToUse.Y = FMath::Clamp(ScalarToUse.Y, MinValueClamp.Y, MaxValueClamp.Y);
			ScalarToUse.Z = FMath::Clamp(ScalarToUse.Z, MinValueClamp.Z, MaxValueClamp.Z);
			
			return CurrentValue.Get<FVector>() * ScalarToUse;
		}
	}
	
	return CurrentValue;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UGridInputModifierAimInversion

FInputActionValue UGridInputModifierAimInversion::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
	FInputActionValue CurrentValue, float DeltaTime)
{
	UGridLocalPlayer* LocalPlayer = GridInputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (!LocalPlayer)
	{
		return CurrentValue;
	}
	
	UGridSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();
	
	if (Settings->GetInvertVerticalAxis())
	{
		NewValue.Y *= -1.0f;
	}
	
	if (Settings->GetInvertHorizontalAxis())
	{
		NewValue.X *= -1.0f;
	}
	
	return NewValue;
}
