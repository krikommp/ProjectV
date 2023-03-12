// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerSpawningManagerComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "GridPlayerStart.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerSpawning, Log, All);

UGridPlayerSpawningManagerComponent::UGridPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(false);
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UGridPlayerSpawningManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ThisClass::OnLevelAdded);

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<AGridPlayerStart> It(World); It; ++It)
	{
		if (AGridPlayerStart* PlayerStart = *It)
		{
			CachedPlayerStarts.Add(PlayerStart);
		}
	}
}

void UGridPlayerSpawningManagerComponent::OnLevelAdded(ULevel* InLevel, UWorld* InWorld)
{
	if (InWorld == GetWorld())
	{
		for (AActor* Actor : InLevel->Actors)
		{
			if (AGridPlayerStart* PlayerStart = Cast<AGridPlayerStart>(Actor))
			{
				ensure(!CachedPlayerStarts.Contains(PlayerStart));
				CachedPlayerStarts.Add(PlayerStart);
			}
		}
	}
}

void UGridPlayerSpawningManagerComponent::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (AGridPlayerStart* PlayerStart = Cast<AGridPlayerStart>(SpawnedActor))
	{
		CachedPlayerStarts.Add(PlayerStart);
	}
}

// AGridGameMode Proxied Calls - Need to handle when someone chooses
// to restart a player the normal way in the engine.
//======================================================================

AActor* UGridPlayerSpawningManagerComponent::ChoosePlayerStart(AController* Player)
{
	if (Player)
	{
#if WITH_EDITOR
		if (APlayerStart* PlayerStart = FindPlayFromHereStart(Player))
		{
			return PlayerStart;
		}
#endif

		TArray<AGridPlayerStart*> StarterPoints;
		for (auto StartIt = CachedPlayerStarts.CreateIterator(); StartIt; ++StartIt)
		{
			if (AGridPlayerStart* Start = (*StartIt).Get())
			{
				StarterPoints.Add(Start);
			}
			else
			{
				StartIt.RemoveCurrent();
			}
		}

		if (APlayerState* PlayerState = Player->GetPlayerState<APlayerState>())
		{
			// start dedicated spectators at any random starting location, but they do not claim it
			if (PlayerState->IsOnlyASpectator())
			{
				if (!StarterPoints.IsEmpty())
				{
					return StarterPoints[FMath::RandRange(0, StarterPoints.Num() - 1)];
				}

				return nullptr;
			}
		}

		AActor* PlayerStart = OnChoosePlayerStart(Player, StarterPoints);

		if (!PlayerStart)
		{
			PlayerStart = GetFirstRandomUnoccupiedPlayerStart(Player, StarterPoints);
		}

		if (AGridPlayerStart* GridStart = Cast<AGridPlayerStart>(PlayerStart))
		{
			GridStart->TryClaim(Player);
		}

		return PlayerStart;
	}

	return nullptr;
}

#if WITH_EDITOR
APlayerStart* UGridPlayerSpawningManagerComponent::FindPlayFromHereStart(AController* Player)
{
	// Only 'Play From Here' for a player controller, bots etc. should all spawn from normal spawn points.
	if (Player->IsA<APlayerController>())
	{
		if (UWorld* World = GetWorld())
		{
			for (TActorIterator<APlayerStart> It(World); It; ++It)
			{
				if (APlayerStart* PlayerStart = *It)
				{
					if (PlayerStart->IsA<APlayerStartPIE>())
					{
						// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
						return PlayerStart;
					}
				}
			}
		}
	}

	return nullptr;
}
#endif

bool UGridPlayerSpawningManagerComponent::ControllerCanRestart(AController* Player)
{
	bool bCanRestart = true;

	// TODO Can they restart?

	return bCanRestart;
}

void UGridPlayerSpawningManagerComponent::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	OnFinishRestartPlayer(NewPlayer, StartRotation);
	K2_OnFinishRestartPlayer(NewPlayer, StartRotation);
}

//================================================================

void UGridPlayerSpawningManagerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

APlayerStart* UGridPlayerSpawningManagerComponent::GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<AGridPlayerStart*>& StartPoints) const
{
	if (Controller)
	{
		TArray<AGridPlayerStart*> UnOccupiedStartPoints;
		TArray<AGridPlayerStart*> OccupiedStartPoints;

		for (AGridPlayerStart* StartPoint : StartPoints)
		{
			EGridPlayerStartLocationOccupancy State = StartPoint->GetLocationOccupancy(Controller);

			switch (State)
			{
				case EGridPlayerStartLocationOccupancy::Empty:
					UnOccupiedStartPoints.Add(StartPoint);
					break;
				case EGridPlayerStartLocationOccupancy::Partial:
					OccupiedStartPoints.Add(StartPoint);
					break;

			}
		}

		if (UnOccupiedStartPoints.Num() > 0)
		{
			return UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			return OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
		}
	}

	return nullptr;
}

