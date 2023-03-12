// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerController.h"


AGridPlayerController::AGridPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridReplayPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}
