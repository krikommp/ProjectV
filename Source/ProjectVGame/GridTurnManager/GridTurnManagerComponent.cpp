// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTurnManagerComponent.h"

#include "GridGlobalDelegates.h"
#include "GridLogChannel.h"
#include "GridTraceChannel.h"
#include "AbilitySystem/GridAbilitySystemComponent.h"
#include "ChessPieces/GridChessPiece.h"
#include "Messages/GridVerbMessage.h"
#include "Player/GridPlayerController.h"

UGridTurnManagerComponent::UGridTurnManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TurnCounter = INDEX_NONE;
}

void UGridTurnManagerComponent::OnRegister()
{
	Super::OnRegister();
}

void UGridTurnManagerComponent::RegisterActionUnit(const FGameplayTag& Faction, AGridChessPiece* ChessPiece)
{
	for (auto& ActionQueue : ActionQueues)
	{
		if (ActionQueue.Faction.MatchesTag(Faction))
		{
			ActionQueue.ChessPieces.Add(ChessPiece);
			return;
		}
	}
	ActionQueues.Add({ Faction, TArray<TObjectPtr<AGridChessPiece>>{ ChessPiece } });
}

void UGridTurnManagerComponent::UnRegisterActionUnit(const FGameplayTag& Faction, AGridChessPiece* ChessPiece)
{
	for (int32 Index = 0; Index < ActionQueues.Num();)
	{
		if (ActionQueues[Index].Faction.MatchesTag(Faction))
		{
			ActionQueues[Index].ChessPieces.Remove(ChessPiece);
			if (ActionQueues[Index].ChessPieces.IsEmpty())
			{
				ActionQueues.RemoveAt(Index);
				continue;
			}
		}
		++Index;
	}
}

void UGridTurnManagerComponent::K2_GetCurrentActionQueue(FActionQueue& OutActionQueue)
{
	OutActionQueue = GetCurrentActionQueue();
}

AGridChessPiece* UGridTurnManagerComponent::GetCurrentUnit() const
{
	return CurrentActionPiece;
}

bool UGridTurnManagerComponent::SetCurrentUnit(AGridChessPiece* InChessPiece)
{
	if (InChessPiece == nullptr) return false;
	if (CurrentActionPiece != InChessPiece)
	{
		FGridGlobalDelegates::OnChessPieceSelectChanged.Broadcast(CurrentActionPiece, InChessPiece);
		if (!GetCurrentActionQueue().ChessPieces.Contains(InChessPiece)) return false;
		if (CurrentActionPiece != nullptr)
		{
			CurrentActionPiece->OnChessPieceUnSelect();
		}
		CurrentActionPiece = InChessPiece;
		CurrentActionPiece->OnChessPieceSelect();
		OnSwitchChessPieceSelect.Broadcast();
		return true;
	}
	return false;
}

bool UGridTurnManagerComponent::IsUnitInCurrentActionQueue(AGridChessPiece* InChessPiece)
{
	const auto CurrentActionQueue = GetCurrentActionQueue();
	return CurrentActionQueue.ChessPieces.Contains(InChessPiece);
}

void UGridTurnManagerComponent::StartTurn()
{
	if (TurnCounter == INDEX_NONE || GetCurrentActionIndex() == ActionQueues.Num() - 1)
	{
		OnTurnFinish.Broadcast();
	}
	
	if (TurnCounter != INDEX_NONE)
	{
		// last action queue
		FActionQueue LastActionQueue = ActionQueues[GetCurrentActionIndex()];
		// for each all action chess piece
		for (const auto& ChessPiece : LastActionQueue.ChessPieces)
		{
			ChessPiece->OnTurnOverInternal();
		}

		FGridGlobalDelegates::OnChessPieceTurnOver.Broadcast();
	}
	TurnCounter += 1;
	// step up current action queue
	FActionQueue CurrentActionQueue = ActionQueues[GetCurrentActionIndex()];
	// for each all action chess piece
	for (const auto& ChessPiece : CurrentActionQueue.ChessPieces)
	{
		ChessPiece->OnTurnStartInternal();
		ChessPiece->GetGridAbilitySystemComponent()->TickAbilityTurn(1);
	}

	if (GetCurrentActionIndex() == 0)
	{
		OnTurnStart.Broadcast();
	}

	bIsTurnOver = false;
}

AGridChessPiece* UGridTurnManagerComponent::GetChessPieceFormMouseClick(APlayerController* InPlayerController)
{
	FHitResult Hit;
	InPlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ChessPieceTrace), false, Hit);
	if (!Hit.bBlockingHit) return nullptr;
	return Cast<AGridChessPiece>(Hit.GetActor());
}
