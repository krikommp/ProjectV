// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessTeamComponent.h"

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
