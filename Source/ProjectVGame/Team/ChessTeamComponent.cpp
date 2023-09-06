// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessTeamComponent.h"

#include "GridComponents.h"
#include "Chess/GridChessBase.h"
#include "Chess/GridChessData.h"
#include "Chess/GridChessExtensionComponent.h"
#include "GameModes/GridGameState.h"
#include "Tilemap/TilemapStateComponent.h"

UChessTeamComponent::UChessTeamComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UChessTeamComponent::OnRegister()
{
	Super::OnRegister();
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

void UChessTeamComponent::RemoveTeamMember(const ETeamType Team, AGridChessBase* Member)
{
	if (!Teams.Contains(Team))
	{
		return;
	}
	Teams[Team].Remove(Member);
}

bool UChessTeamComponent::CheckChessInTeam(const ETeamType Team, const FName ChessID) const
{
	if (!Teams.Contains(Team))
		return false;

	for (auto& Chess : Teams[Team])
	{
		if (const UGridChessExtensionComponent* Extension = Chess->FindComponentByClass<UGridChessExtensionComponent>())
		{
			if (Extension->GetChessID() == ChessID)
				return true;
		}
	}

	return false;
}
