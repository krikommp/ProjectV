// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBlueprintLibrary.h"

#include "ChessTurnComponent.h"
#include "Team/ChessTeamComponent.h"

ETeamType UTurnBlueprintLibrary::GetCurrentTurnTeam(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UChessTurnComponent* TurnComponent = World->GetGameState()->FindComponentByClass<UChessTurnComponent>())
		{
			int TurnCount = TurnComponent->GetTurnCount() % static_cast<int32>(ETeamType::MAX);
			return static_cast<ETeamType>(TurnCount);
		}
	}
	return ETeamType::Player;
}

TArray<AGridChessBase*> UTurnBlueprintLibrary::GetCurrentTurnChess(const UObject* WorldContextObject)
{
	TArray<AGridChessBase*> ChessBases;
	const ETeamType Team = GetCurrentTurnTeam(WorldContextObject);

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UChessTeamComponent* TeamComponent = World->GetGameState()->FindComponentByClass<UChessTeamComponent>())
		{
			const int32 Count = TeamComponent->GetTeamLength(Team);
			for (int32 i = 0; i < Count; ++i)
			{
				ChessBases.Add(TeamComponent->GetMember(Team, i));
			}
		}
	}

	return ChessBases;
}
