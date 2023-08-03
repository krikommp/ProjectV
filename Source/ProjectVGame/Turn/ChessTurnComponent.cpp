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

	if (UTilemapStateComponent* TilemapStateComponent = FIND_STATE_COMP_IN_STATE(AGridGameState, TilemapStateComponent))
	{
		TilemapStateComponent->CallOrRegister_OnSpawnFinished(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnTilemapLoaded));
	}
}

void UChessTurnComponent::OnTilemapLoaded()
{
	TurnCount = 0;
	bReadyToStartTurn = true;

	// todo... 模拟一个开始回合逻辑，这里需要放到关卡中编辑
	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ThisClass::StartTurn, 2.0f, false);
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
