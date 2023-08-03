// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncAction_TurnReady.h"

#include "ChessTurnComponent.h"
#include "GridComponents.h"

UAsyncAction_TurnReady::UAsyncAction_TurnReady(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UAsyncAction_TurnReady* UAsyncAction_TurnReady::WaitForTurnReady(UObject* InWorldContextObject)
{
	UAsyncAction_TurnReady* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_TurnReady>();
		Action->WorldPtr = World;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncAction_TurnReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToTurnReady(GameState);
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

void UAsyncAction_TurnReady::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToTurnReady(GameState);
}

void UAsyncAction_TurnReady::Step2_ListenToTurnReady(AGameStateBase* GameState)
{
	check(GameState);
	UChessTurnComponent* TurnComponent = FIND_STATE_COMP_IN_PAWN(ChessTurnComponent);
	check(TurnComponent);

	if (TurnComponent->IsReady())
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
		TurnComponent->CallOrRegister_OnTurnReady(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleTurnReady));
	}
}

void UAsyncAction_TurnReady::Step3_HandleTurnReady()
{
	Step4_BroadcastReady();
}

void UAsyncAction_TurnReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();

	SetReadyToDestroy();
}
