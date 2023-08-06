// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessMovementComponent.h"

#include "GridComponents.h"
#include "Tilemap/TilemapExtensionComponent.h"

class UTilemapExtensionComponent;

UGridChessMovementComponent::UGridChessMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UGridChessMovementComponent::SetMoveTarget(const FVector& Target)
{
	APawn* Pawn = GetPawn<APawn>();

	Pawn->SetActorLocation(Target);

	OnChessMovementFinished.Broadcast();
	OnChessMovementFinished.Clear();
}

void UGridChessMovementComponent::MoveToPathfinding(int32 Index)
{
	const APawn* Pawn = GetPawn<APawn>();

	UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
	{
		return;
	}

	TilemapExtensionComponent->SetPathfindingIndex(Index);

	const FVector Target = TilemapExtensionComponent->GetPathfindingBlockLocation(Index);
	
	SetMoveTarget(Target);
}

void UGridChessMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGridChessMovementComponent::Register_OnChessMovementFinished(FOnChessMovementFinished::FDelegate&& Delegate)
{
	OnChessMovementFinished.Add(Delegate);
}
