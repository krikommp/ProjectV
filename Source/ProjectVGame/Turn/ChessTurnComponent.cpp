// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessTurnComponent.h"

#include "GridComponents.h"
#include "GameModes/GridGameState.h"
#include "Team/TeamType.h"
#include "Tilemap/TilemapStateComponent.h"

UChessTurnComponent::UChessTurnComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer), bReadyToStartTurn(false), TurnCount(0)
{
}

void UChessTurnComponent::OnRegister()
{
	Super::OnRegister();

	// print game state is valid
	if (AGridGameState* GameState = Cast<AGridGameState>(GetWorld()->GetGameState()))
	{
		UE_LOG(LogTemp, Error, TEXT("Game State is valid"));
	}
	if (UTilemapStateComponent* TilemapStateComponent = FIND_STATE_COMP_IN_STATE(AGridGameState, TilemapStateComponent))
	{
		TilemapStateComponent->CallOrRegister_OnSpawnFinished(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnTilemapLoaded));
	}
}

void UChessTurnComponent::CallOrRegister_OnTurnReady(FSimpleMulticastDelegate::FDelegate&& Delegate)
{
	if (bReadyToStartTurn)
	{
		Delegate.Execute();
	}else
	{
		OnReady.Add(Delegate);
	}
}

void UChessTurnComponent::OnTilemapLoaded()
{
	TurnCount = 0;
	bReadyToStartTurn = true;

	OnReady.Broadcast();
	OnReady.Clear();
}

void UChessTurnComponent::StartTurn()
{
	UE_LOG(LogTemp, Error, TEXT("Start Turn..."));
}

void UChessTurnComponent::SwitchTurn()
{
	++TurnCount;
}

bool UChessTurnComponent::CheckIsPlayerTurn() const
{
	return static_cast<int>(ETeamType::Player) == TurnCount % static_cast<int>(ETeamType::MAX);
}
