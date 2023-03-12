// Fill out your copyright notice in the Description page of Project Settings.


#include "GridChessPieceComponent.h"

#include "GridChessPiece.h"
#include "GridChessPieceData.h"
#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "GridChessPieceExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Heros/GridHeroInfo.h"
#include "Player/GridLocalPlayer.h"
#include "System/GridAssetManager.h"
#include "System/GridGameInstance.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

const FName UGridChessPieceComponent::NAME_ActorFeatureName("ChessPiece");
const FName UGridChessPieceComponent::Name_LoadPieceInfoNow("LoadPieceInfo");

UGridChessPieceComponent::UGridChessPieceComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToLoadPieceInfo = false;
}

void UGridChessPieceComponent::OnRegister()
{
	Super::OnRegister();
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogGrid, Error,
		       TEXT(
			       "[GridChessPieceComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GridChessPieceComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("GridChessPieceComponent");

			FMessageLog(HeroMessageLogName).Error()
			                               ->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
			                               ->AddToken(FTextToken::Create(Message));

			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UGridChessPieceComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                  FGameplayTag DesiredState) const
{
	check(Manager);

	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// pawn 生成完毕，准备进入下一步初始化流程
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		return true;
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		return Manager->HasFeatureReachedInitState(Pawn, UGridChessPieceExtensionComponent::NAME_ActorFeatureName,
		                                           InitTags.InitState_DataInitialized);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		// todo...
		return true;
	}
	return false;
}

void UGridChessPieceComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                     FGameplayTag DesiredState)
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	if (CurrentState == FGridGameplayTags::Get().InitState_DataAvailable && DesiredState == FGridGameplayTags::Get().
		InitState_DataInitialized)
	{
		AGridChessPiece* ChessPiece = GetPawn<AGridChessPiece>();
		if (!ensure(ChessPiece))
		{
			return;
		}

		if (UGridChessPieceExtensionComponent* ChessPieceExtComp =
			UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(ChessPiece))
		{
			const UGridChessPieceData* ChessPieceData = nullptr;
			ChessPieceData = ChessPieceExtComp->GetChessPieceData<UGridChessPieceData>();

			ChessPieceExtComp->InitializeAbilitySystem(ChessPiece->GetGridAbilitySystemComponent(), ChessPiece);

			if (ChessPieceData != nullptr)
			{
				const auto LocalPlayer = Cast<UGridLocalPlayer>(
					GetGameInstance<UGridGameInstance>()->GetLocalPlayerByIndex(ChessPieceData->PlayerIndex));
				const auto HeroInfo = InitializePieceInfo(LocalPlayer, ChessPieceData->PieceID);
				ChessPieceExtComp->InitializeChessPieceInfo(LocalPlayer, HeroInfo);
				ChessPieceExtComp->InitializeAbilitySet();
				bReadyToLoadPieceInfo = true;
				OnChessPieceInitialized.Broadcast();
			}
		}
	}
}

void UGridChessPieceComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGridChessPieceExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGridGameplayTags::Get().InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UGridChessPieceComponent::CheckDefaultInitialization()
{
	const FGridGameplayTags& InitTags = FGridGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = {
		InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized,
		InitTags.InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UGridChessPieceComponent::CallOrRegister_ChessPieceInitialized(FSimpleMulticastDelegate::FDelegate&& Delegate)
{
	if (bReadyToLoadPieceInfo)
	{
		Delegate.Execute();
		return;
	}

	if (!OnChessPieceInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnChessPieceInitialized.Add(Delegate);
	}
}

void UGridChessPieceComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UGridChessPieceExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(FGridGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UGridChessPieceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

UGridHeroInfo* UGridChessPieceComponent::InitializePieceInfo(const UGridLocalPlayer* InLocalPlayer, int32 HeroId)
{
	const APawn* Owner = GetPawnChecked<APawn>();
	
	if (InLocalPlayer == nullptr)
	{
		UE_LOG(LogGrid, Log, TEXT("InValid Player to set, i will get hero info by default"));
		const UGridAssetManager& AssetManager = UGridAssetManager::Get();
		return UGridHeroInfo::CreateHeroInfo(AssetManager.GetHeroData(HeroId));
	}else
	{
		return InLocalPlayer->GetHeroInfo(HeroId);
	}
}
