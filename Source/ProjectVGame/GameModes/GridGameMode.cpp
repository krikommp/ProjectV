// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameMode.h"

#include "GridExperienceManagerComponent.h"
#include "GridGameState.h"
#include "GridGlobalDelegates.h"
#include "GridLogChannel.h"
#include "GridWorldSettings.h"
#include "Character/GridCharacter.h"
#include "Character/GridPawnExtensionComponent.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GridPlayerController.h"
#include "Player/GridPlayerSpawningManagerComponent.h"
#include "Player/GridPlayerState.h"
#include "System/GridAssetManager.h"
#include "System/GridGameSession.h"
#include "UI/GridHUD.h"

AGridGameMode::AGridGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	GameStateClass = AGridGameState::StaticClass();
	GameSessionClass = AGridGameSession::StaticClass();
	PlayerControllerClass = AGridPlayerController::StaticClass();
	ReplaySpectatorPlayerControllerClass = AGridReplayPlayerController::StaticClass();
	PlayerStateClass = AGridPlayerState::StaticClass();
	DefaultPawnClass = AGridCharacter::StaticClass();
	HUDClass = AGridHUD::StaticClass();
}

const UGridPawnData* AGridGameMode::GetPawnDataForController(const AController* InController) const
{
	// 如果 PlayerState 中已经拥有了 PawnData
	if (InController != nullptr)
	{
		if (const AGridPlayerState* GridPS = InController->GetPlayerState<AGridPlayerState>())
		{
			if (const UGridPawnData* PawnData = GridPS->GetPawnData<UGridPawnData>())
			{
				return PawnData;
			}
		}
	}

	// 如果没有，那么就从当前 Experience 中获取默认 PawnData
	check(GameState);
	UGridExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UGridExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// 如果 Experience 没有重载 PawnData， 那么就获取全局默认的 PawnData
		return UGridAssetManager::Get().GetDefaultPawnData();
	}

	return nullptr;
}

void AGridGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AGridGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	UWorld* World = GetWorld();

	// 从配置中加载
	// 通常采用这种方式加载，需要定义 UserFacingExperienceDefinition 并指定额外参数
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UGridExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// 尝试从命令行中获取指定的 Experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceCommandLine);
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// 尝试从 World Setting 中获取默认 Experience
	if (!ExperienceId.IsValid())
	{
		if (AGridWorldSettings* TypedWorldSettings = Cast<AGridWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	// 根据 ExperienceId 查找实际的 Experience 资源是否存在
	UGridAssetManager& AssetManager = UGridAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /* OUT */ Dummy))
	{
		UE_LOG(LogGridExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
        ExperienceId = FPrimaryAssetId();
	}

	// fallback experience
	// 当没有进行任何 Experience 的配置时，调用 B_GridDefaultExperience
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("GridExperienceDefinition"), FName("B_GridDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void AGridGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
#if WITH_SERVER_CODE
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogGridExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UGridExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>();
		check(ExperienceManagerComponent);
		ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
	}else
	{
		UE_LOG(LogGridExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
#endif
}

void AGridGameMode::OnExperienceLoaded(const UGridExperienceDefinition* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool AGridGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UGridExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AGridGameMode::HandleInputModeChanged(ECommonInputMode InputMode)
{
	FGridGlobalDelegates::OnActiveInputModeChanged.Broadcast(InputMode);
}

UClass* AGridGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UGridPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AGridGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UGridPawnExtensionComponent* PawnExtComp = UGridPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UGridPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogGrid, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogGrid, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogGrid, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool AGridGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

void AGridGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* AGridGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (UGridPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UGridPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ChoosePlayerStart(Player);
	}
	
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AGridGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (UGridPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UGridPlayerSpawningManagerComponent>())
	{
		PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	}
	
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}


bool AGridGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool AGridGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	if (UGridPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UGridPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ControllerCanRestart(Controller);
	}

	return true;
}

void AGridGameMode::InitGameState()
{
	Super::InitGameState();

	UGridExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UGridExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AGridGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const ULocalPlayer* Player = NewPlayer->GetLocalPlayer();
	if (const UCommonUIActionRouterBase* CommonUIActionRouterBase = Player->GetSubsystem<UCommonUIActionRouterBase>())
	{
		if (!CommonUIActionRouterBase->OnActiveInputModeChanged().IsBoundToObject(Player))
		{
			CommonUIActionRouterBase->OnActiveInputModeChanged().AddUObject(this, &ThisClass::HandleInputModeChanged);
		}
	}
}

void AGridGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	OnGameModeCombinedPostLoginDelegate.Broadcast(this, NewPlayer);
}

void AGridGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

bool AGridGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	return true;
}

void AGridGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(LogGrid, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogGrid, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}

















