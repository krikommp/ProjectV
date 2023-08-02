// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessExtensionComponent.h"

#include "GridChessBase.h"
#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GridChessData.h"
#include "GameModes/GridGameState.h"
#include "Heros/GridHeroInfo.h"
#include "Player/GridLocalPlayer.h"
#include "System/GridAssetManager.h"
#include "System/GridGameInstance.h"
#include "Tilemap/TilemapStateComponent.h"

const FName UGridChessExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UGridChessExtensionComponent::UGridChessExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ChessData = nullptr;
	
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UGridChessExtensionComponent::SetChessData(const UGridChessData* InData)
{
	check(InData);

	if (ChessData)
	{
		APawn* Pawn = GetPawnChecked<APawn>();
		UE_LOG(LogGrid, Error, TEXT("Trying to set ChessData [%s] on pawn [%s] that already has valid ChessData [%s]."), *GetNameSafe(InData), *GetNameSafe(Pawn), *GetNameSafe(ChessData));
		return;
	}

	ChessData = InData;

	CheckDefaultInitialization();
}

void UGridChessExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("UGridChessExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UGridChessExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one UGridChessExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();

	if (UTilemapStateComponent* TilemapStateComponent = GetWorld()->GetGameState()->FindComponentByClass<UTilemapStateComponent>())
	{
		TilemapStateComponent->CallOrRegister_OnChessSpawn(FOnTilemapSpawnChess::FDelegate::CreateUObject(this, &ThisClass::OnChessSpawn));
	}
}

void UGridChessExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听所有 GameFeature 的变化
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UGridChessExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UGridChessExtensionComponent::OnChessSpawn(const FTilemapSpawnParameters& Parameters)
{
	if (Parameters.Chess != GetPawn<AGridChessBase>())
		return;
	SetChessData(Parameters.ChessData);
}

bool UGridChessExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
                                                      FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!ChessData)
		{
			return false;
		}
		return true;
	}else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void UGridChessExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == FGridGameplayTags::Get().InitState_DataInitialized)
	{
		// todo...
		check(ChessData);

		// todo... 临时方法，通过Data上的Team来判断是否是玩家可控制对象
		if (ChessData->Team == ETeamType::Player)
		{
			const auto LocalPlayer = Cast<UGridLocalPlayer>(GetGameInstance<UGridGameInstance>()->GetFirstGamePlayer());
			ChessInfo = LocalPlayer->GetHeroInfo(ChessData->ChessID);
		}else
		{
			const UGridAssetManager& AssetManager = UGridAssetManager::Get();
			ChessInfo = UGridHeroInfo::CreateHeroInfo(AssetManager.GetHeroData(ChessData->ChessID));
		}
		
		const AGridChessBase* Chess = GetPawnChecked<AGridChessBase>();
		Chess->SetupSkeletalMeshAsset(ChessInfo->HeroData.SkeletalMesh);
		Chess->SetupAnimInstanceClass(ChessInfo->HeroData.AnimBlueprint);
	}
}

void UGridChessExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UGridChessExtensionComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}
