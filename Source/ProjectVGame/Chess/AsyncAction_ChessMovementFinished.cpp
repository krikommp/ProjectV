// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncAction_ChessMovementFinished.h"

#include "GridChessBase.h"
#include "GridChessMovementComponent.h"
#include "GridComponents.h"
#include "GameFramework/GameStateBase.h"

UAsyncAction_ChessMovementFinished::UAsyncAction_ChessMovementFinished(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncAction_ChessMovementFinished* UAsyncAction_ChessMovementFinished::WaitForChessMovementFinished(UObject* WorldContextObject, AGridChessBase* InChess)
{
	UAsyncAction_ChessMovementFinished* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_ChessMovementFinished>();
		Action->WorldPtr = World;
		Action->Chess = InChess;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncAction_ChessMovementFinished::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToChessMovementFinished(GameState);
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

void UAsyncAction_ChessMovementFinished::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}
	Step2_ListenToChessMovementFinished(GameState);
}

void UAsyncAction_ChessMovementFinished::Step2_ListenToChessMovementFinished(AGameStateBase* GameState)
{
	check(Chess.IsValid());
	
	UGridChessMovementComponent* ChessMovementComponent = FIND_PAWN_COMP(GridChessMovementComponent, Chess.Get());
	check(ChessMovementComponent);

	ChessMovementComponent->Register_OnChessMovementFinished(FOnChessMovementFinished::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleChessMovementFinished));
}

void UAsyncAction_ChessMovementFinished::Step3_HandleChessMovementFinished()
{
	Step4_BroadcastFinished();
}

void UAsyncAction_ChessMovementFinished::Step4_BroadcastFinished()
{
	OnFinished.Broadcast();
	SetReadyToDestroy();
}

