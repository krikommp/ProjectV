// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapDrawRangeComponent.h"

#include "GridComponents.h"
#include "TilemapExtensionComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

class UTilemapExtensionComponent;

UTilemapDrawRangeComponent::UTilemapDrawRangeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;	
}

void UTilemapDrawRangeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTilemapDrawRangeComponent::DisplayPathfindingDecal(TArray<int32> Indexes)
{
	const APawn* Pawn = GetPawnChecked<APawn>();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
		return;

	for (const int32 Index : Indexes)
	{
		const FVector DecalLocation = TilemapExtensionComponent->GetTilemap()->GetPathfindingBlockLocation(Index);
		UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), TilemapExtensionComponent->GetTilemap()->GetTilemap()->MoveRangeMat, DecalSize, DecalLocation, {90.0f, 0.0f, 0.0f});

		DecalComponents.Add(DecalComponent);
	}
}

void UTilemapDrawRangeComponent::ClearAllDecalComponents()
{
	for (const auto& DecalComp : DecalComponents)
	{
		DecalComp->DestroyComponent();
	}
	DecalComponents.Empty();
}
