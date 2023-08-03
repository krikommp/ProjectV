// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapVisibleComponent.h"

#include "TilemapExtensionComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

UTilemapVisibleComponent::UTilemapVisibleComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;	
}

void UTilemapVisibleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTilemapVisibleComponent::DisplayPathfindingDecal(int32 Index, UMaterialInterface* Material)
{
	const APawn* Pawn = GetPawnChecked<APawn>();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
		return;

	const FVector DecalLocation = TilemapExtensionComponent->GetTilemap()->GetPathfindingBlockLocation(Index);
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Material, DecalSize, DecalLocation, {90.0f, 0.0f, 0.0f});

	DecalComponents.Add(DecalComponent);
}

void UTilemapVisibleComponent::ClearAllDecalComponents()
{
	for (const auto& DecalComp : DecalComponents)
	{
		DecalComp->DestroyComponent();
	}
	DecalComponents.Empty();
}
