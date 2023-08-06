// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessTeamComponent.h"

#include "GridComponents.h"
#include "Chess/GridChessBase.h"
#include "Chess/GridChessData.h"
#include "GameModes/GridGameState.h"
#include "Tilemap/TilemapStateComponent.h"

UChessTeamComponent::UChessTeamComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UChessTeamComponent::OnRegister()
{
	Super::OnRegister();

	if (UTilemapStateComponent* TilemapStateComponent = FIND_STATE_COMP_IN_STATE(AGridGameState, TilemapStateComponent))
	{
		TilemapStateComponent->CallOrRegister_OnChessSpawn(FOnTilemapSpawnChess::FDelegate::CreateUObject(this, &ThisClass::OnChessSpawn));
	}
}

void UChessTeamComponent::OnChessSpawn(const FTilemapSpawnParameters& Parameters)
{
	AddTeamMember(Parameters.ChessData->Team, Parameters.Chess);
}

void UChessTeamComponent::AddTeamMember(const ETeamType Team, AGridChessBase* Member)
{
	if (!Teams.Contains(Team))
	{
		Teams.Add(Team, TArray<TWeakObjectPtr<AGridChessBase>>{});
	}

	Teams[Team].Add(Member);
}

void UChessTeamComponent::AddTeamMember(const ETeamType Team, const TArray<AGridChessBase*>& Members)
{
	for (auto& Member : Members)
	{
		AddTeamMember(Team, Member);
	}
}
