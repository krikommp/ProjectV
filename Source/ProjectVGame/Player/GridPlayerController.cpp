// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerController.h"
#include "GridCheatManager.h"


AGridPlayerController::AGridPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
#if USING_CHEAT_MANAGER
	CheatClass = UGridCheatManager::StaticClass();
#endif // #if USING_CHEAT_MANAGER
}

void AGridReplayPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}
