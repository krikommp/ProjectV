// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Array.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "GridInputConfig.h"
#include "HAL/Platform.h"
#include "InputTriggers.h"
#include "Misc/AssertionMacros.h"
#include "UObject/UObjectGlobals.h"
#include "GridInputComponent.generated.h"

class UGridInputConfig;
class UEnhancedInputLocalPlayerSubsystem;

/**
 * UGridInputComponent
 *
 * 用于管理输入映射和绑定输入配置资源
 */
UCLASS(Config = Input)
class PROJECTVGAME_API UGridInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UGridInputComponent(const FObjectInitializer& ObjectInitializer);

	void AddInputMappings(const UGridInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UGridInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UGridInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UGridInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template<class UserClass, typename FuncType>
void UGridInputComponent::BindNativeAction(const UGridInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UGridInputComponent::BindAbilityActions(const UGridInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FGridInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
