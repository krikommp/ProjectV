// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncAction_ChessPieceSpawnFinish.h"

#include "GridChessPieceSpawnComponent.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ChessPieceSpawnFinish)

// UAsyncAction_ChessPieceSpawnFinish::UAsyncAction_ChessPieceSpawnFinish(const FObjectInitializer& ObjectInitializer)
// 	:Super(ObjectInitializer)
// {
// }

UAsyncAction_ChessPieceSpawnFinish* UAsyncAction_ChessPieceSpawnFinish::WaitForChessPieceSpawnFinish(
	UObject* InWorldContextObject)
{
	UAsyncAction_ChessPieceSpawnFinish* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_ChessPieceSpawnFinish>();
		Action->WorldPtr = World;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncAction_ChessPieceSpawnFinish::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToChessPieceSpawn(GameState);
		}else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step2_ListenToChessPieceSpawn);
		}
	}else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_ChessPieceSpawnFinish::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToChessPieceSpawn(GameState);
}

void UAsyncAction_ChessPieceSpawnFinish::Step2_ListenToChessPieceSpawn(AGameStateBase* GameState)
{
	check(GameState);
	UGridChessPieceSpawnComponent* ChessPieceSpawnComponent = GameState->FindComponentByClass<UGridChessPieceSpawnComponent>();
	check(ChessPieceSpawnComponent);

	if (ChessPieceSpawnComponent->IsSpawnFinished())
	{
		UWorld* World = GameState->GetWorld();
		check(World);

		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady));
	}else
	{
		ChessPieceSpawnComponent->CallOrRegister_OnChessPieceSpawnFinished(FOnGridChessPieceSpawnFinished::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleSpawnFinished));
	}
}

void UAsyncAction_ChessPieceSpawnFinish::Step3_HandleSpawnFinished(const TArray<AGridChessPiece*>& InChessPieceArray)
{
	Step4_BroadcastReady();
}

void UAsyncAction_ChessPieceSpawnFinish::Step4_BroadcastReady()
{
	OnFinish.Broadcast();

	SetReadyToDestroy();
}


