// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncAction_ExperienceReady.h"

#include "GridExperienceDefinition.h"
#include "GridExperienceManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ExperienceReady)

UAsyncAction_ExperienceReady::UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}


UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* InWorldContextObject, const EExperienceLoadPriority ExperienceLoadPriority)
{
	UAsyncAction_ExperienceReady* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_ExperienceReady>();
		Action->WorldPtr = World;
		Action->LoadPriority = ExperienceLoadPriority;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncAction_ExperienceReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToExperienceLoading(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step1_HandleGameStateSet);
		}
	}
	else
	{
		// No world so we'll never finish naturally
		SetReadyToDestroy();
	}
}

void UAsyncAction_ExperienceReady::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToExperienceLoading(GameState);
}

void UAsyncAction_ExperienceReady::Step2_ListenToExperienceLoading(AGameStateBase* GameState)
{
	check(GameState);
	UGridExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		UWorld* World = GameState->GetWorld();
		check(World);

		// The experience happened to be already loaded, but still delay a frame to
		// make sure people don't write stuff that relies on this always being true
		//@TODO: Consider not delaying for dynamically spawned stuff / any time after the loading screen has dropped?
		//@TODO: Maybe just inject a random 0-1s delay in the experience load itself?
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady));
	}
	else
	{
		switch (LoadPriority)
		{
		case EExperienceLoadPriority::EEP_Low:
			ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
			break;
		case EExperienceLoadPriority::EEP_Normal:
			ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
			break;
		case EExperienceLoadPriority::EEP_High:
			ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
			break;
		default: ;
		}
	}
}

void UAsyncAction_ExperienceReady::Step3_HandleExperienceLoaded(const UGridExperienceDefinition* CurrentExperience)
{
	Step4_BroadcastReady();
}

void UAsyncAction_ExperienceReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();

	SetReadyToDestroy();
}
