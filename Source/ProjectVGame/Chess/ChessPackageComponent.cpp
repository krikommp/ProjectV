// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPackageComponent.h"

#include "ChessPackageInfo.h"
#include "GridComponents.h"
#include "GridLogChannel.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "Heros/GridHeroInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GridLocalPlayer.h"
#include "Player/GridPlayerState.h"
#include "Settings/GridSettingDefaultValues.h"
#include "System/GridAssetManager.h"

UChessPackageComponent::UChessPackageComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer), bChessInfoLoaded(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UChessPackageComponent* UChessPackageComponent::FindChessPackageComponent(const ULocalPlayer* LocalPlayer)
{
	const APlayerState* PlayerState = LocalPlayer->PlayerController->PlayerState;
	return PlayerState ? PlayerState->FindComponentByClass<UChessPackageComponent>() : nullptr;
}

UChessPackageInfo* UChessPackageComponent::GetChessPackageInfo() const
{
	if (ChessPackageInfo == nullptr)
		return nullptr;
	return ChessPackageInfo;
}

UGridHeroInfo* UChessPackageComponent::GetChessInfo(const FName& ChessID) const
{
	if (ChessInfoMap.Contains(ChessID))
	{
		return ChessInfoMap[ChessID];
	}
	return nullptr;
}

void UChessPackageComponent::OnRegister()
{
	Super::OnRegister();

	if (UGridExperienceManagerComponent* ExperienceManagerComponent = FIND_STATE_COMP_IN_PAWN(GridExperienceManagerComponent))
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("[UChessPackageComponent::OnRegister]: Can not found %s on Game State"), *GetNameSafe(this));
	}
}

void UChessPackageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UChessPackageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UChessPackageComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (bChessInfoLoaded)
	{
		OutReason = TEXT("Chess Package Info still loading.");
		return true;
	}
	return false;
}

void UChessPackageComponent::OnExperienceLoaded(const UGridExperienceDefinition* Experience)
{
	// todo...
	const UGridSettingDefaultValues *DefaultValues = GetMutableDefault<UGridSettingDefaultValues>();

	const FString PlayerName = DefaultValues->DefaultPlayerName;
	const FString SlotName = FString::Format(TEXT("CessPackageInfo_{0}"), {PlayerName});

	const UGridLocalPlayer* Player = Cast<UGridLocalPlayer>(GetPlayerStateChecked<AGridPlayerState>()->GetPlayerController()->Player);
	check(Player);

	if (UGameplayStatics::DoesSaveGameExist(SlotName, Player->GetLocalPlayerIndex()))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, Player->GetLocalPlayerIndex(), FAsyncLoadGameFromSlotDelegate::CreateUObject(this, &UChessPackageComponent::Step1_HandleAsyncLoad));
	}else
	{
		UChessPackageInfo* NewSaveGame = Cast<UChessPackageInfo>(UGameplayStatics::CreateSaveGameObject(UChessPackageInfo::StaticClass()));
		NewSaveGame->ChessIDList = DefaultValues->DefaultChessIDs;
		NewSaveGame->SlotName = SlotName;
		Step1_HandleAsyncLoad(SlotName, Player->GetLocalPlayerIndex(), NewSaveGame);
	}
}

void UChessPackageComponent::Step1_HandleAsyncLoad(const FString& InSlotName, const int32 InUserIndex, USaveGame* LoadedSave)
{
	check(LoadedSave);

	ChessPackageInfo = Cast<UChessPackageInfo>(LoadedSave);

	Step2_LoadChessInfo();
}

void UChessPackageComponent::Step2_LoadChessInfo()
{
	check(ChessPackageInfo);

	const UGridLocalPlayer* Player = Cast<UGridLocalPlayer>(GetPlayerStateChecked<AGridPlayerState>()->GetPlayerController()->Player);
	check(Player);

	for (const FName& ChessID : ChessPackageInfo->ChessIDList)
	{
		const UGridAssetManager& AssetManager = UGridAssetManager::Get();
		const FGridHeroData& HeroData = AssetManager.GetHeroData(ChessID);
		UGridHeroInfo* HeroInfo = UGridHeroInfo::LoadOrCreateHeroInfo(HeroData, Player);
		ChessInfoMap.Add(ChessID, HeroInfo);
	}

	bChessInfoLoaded = true;

	OnChessPackageLoaded.Broadcast(ChessPackageInfo);
	OnChessPackageLoaded.Clear();
}

void UChessPackageComponent::CallOrRegister_OnChessPackageLoaded(FOnChessPackageLoaded::FDelegate&& Delegate)
{
	if (bChessInfoLoaded)
	{
		Delegate.Execute(ChessPackageInfo);
	}else
	{
		OnChessPackageLoaded.Add(Delegate);
	}
}
