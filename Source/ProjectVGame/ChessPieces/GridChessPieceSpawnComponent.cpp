// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceSpawnComponent.h"

#include "GridChessPiece.h"
#include "GridChessPieceExtensionComponent.h"
#include "GridChessPieceStart.h"
#include "GridLogChannel.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameState.h"
#include "Kismet/GameplayStatics.h"

UGridChessPieceSpawnComponent::UGridChessPieceSpawnComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ChessPiecesList.Empty();
	bSpawnInitialized = false;
}

void UGridChessPieceSpawnComponent::OnRegister()
{
	Super::OnRegister();

	if (UGridExperienceManagerComponent* ExperienceManagerComponent = GetGameState<AGridGameState>()->FindComponentByClass<UGridExperienceManagerComponent>())
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("[UGridChessPieceSpawnComponent::OnRegister]: can not found %s on Game State."), *GetNameSafe(this));
	}
}

void UGridChessPieceSpawnComponent::CollectAllChessPieceOnMap()
{
	TArray<AActor*> OuterActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridChessPieceStart::StaticClass(), OuterActors);

	for (const auto& Actor : OuterActors)
	{
		if (const AGridChessPieceStart* ChessPieceStart = Cast<AGridChessPieceStart>(Actor))
		{
			AGridChessPiece* NewChessPiece = CreateNewChessPiece(ChessPieceStart);
			if (UGridChessPieceExtensionComponent* ChessPieceExtComp = UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(NewChessPiece))
			{
				ChessPieceExtComp->SetChessPieceData(ChessPieceStart->DefaultChessPieceData);
				ChessPieceExtComp->SetTileIndex(ChessPieceStart->TileIndex);
			}else
			{
				UE_LOG(LogGrid, Error, TEXT("[UGridChessPieceSpawnComponent::CollectAllChessPieceOnMap]: Can not found UGridChessPieceExtensionComponent on [%s]"), *GetNameSafe(NewChessPiece));
			}
			ChessPiecesList.Add(NewChessPiece);
		}
	}

	for (const auto& ChessPiece : ChessPiecesList)
	{
		if (UGridChessPieceComponent* ChessPieceComponent = UGridChessPieceComponent::FindGridChessPieceComponent(ChessPiece))
		{
			ChessPieceComponent->CallOrRegister_ChessPieceInitialized(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnChessPieceInitialized));
		}
	}
}

void UGridChessPieceSpawnComponent::CallOrRegister_OnChessPieceSpawnFinished(
	FOnGridChessPieceSpawnFinished::FDelegate&& Delegate)
{
	if (bSpawnInitialized)
	{
		Delegate.Execute(ChessPiecesList);
	}
	if (!SpawnFinished.IsBoundToObject(Delegate.GetUObject()))
	{
		SpawnFinished.Add(Delegate);
	}
}

AGridChessPiece* UGridChessPieceSpawnComponent::CreateNewChessPiece(const AGridChessPieceStart* ChessPieceStart) const
{
	const FActorSpawnParameters SpawnParameters;
	FTransform SpawnTransform(ChessPieceStart->GetTransform());
	SpawnTransform.SetLocation({ ChessPieceStart->GetCustomGridAnchorTransform().GetLocation().X, ChessPieceStart->GetCustomGridAnchorTransform().GetLocation().Y, ChessPieceStart->GetTransform().GetLocation().Z });
	AGridChessPiece* NewChessPiece = GetWorld()->SpawnActor<AGridChessPiece>(ChessPieceStart->DefaultChessPieceData->ChessPieceClass, SpawnTransform, SpawnParameters);
	return NewChessPiece;
}

void UGridChessPieceSpawnComponent::OnExperienceLoaded(const UGridExperienceDefinition* ExperienceDefinition)
{
	CollectAllChessPieceOnMap();
}

void UGridChessPieceSpawnComponent::OnChessPieceInitialized()
{
	for (const auto& ChessPiece : ChessPiecesList)
	{
		if (const UGridChessPieceComponent* ChessPieceComponent = UGridChessPieceComponent::FindGridChessPieceComponent(ChessPiece))
		{
			if (!ChessPieceComponent->CheckChessPieceInitialized()) return;
		}
	}

	// all chess piece initialized
	bSpawnInitialized = true;
	SpawnFinished.Broadcast(ChessPiecesList);
}
