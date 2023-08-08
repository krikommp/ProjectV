// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapStateComponent.h"

#include "GridLogChannel.h"
#include "Tilemap3DActor.h"
#include "TilemapAsset.h"
#include "TilemapExtensionComponent.h"
#include "Chess/ChessBlueprintLibrary.h"
#include "Chess/GridChessBase.h"
#include "Chess/GridChessData.h"
#include "Chess/GridChessExtensionComponent.h"
#include "GameModes/GridExperienceDefinition.h"
#include "GameModes/GridExperienceManagerComponent.h"
#include "GameModes/GridGameState.h"
#include "Kismet/GameplayStatics.h"
#include "System/GridAssetManager.h"

UTilemapStateComponent::UTilemapStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), TilemapActor(nullptr), bLoadTilemapFinished(false)
{
}

void UTilemapStateComponent::OnRegister()
{
	Super::OnRegister();

	if (UGridExperienceManagerComponent* ExperienceManagerComponent = FIND_STATE_COMP_IN_STATE(AGridGameState, GridExperienceManagerComponent))
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnGridExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}else
	{
		UE_LOG(LogGrid, Error, TEXT("[UTilemapStateComponent::OnRegister]: Can not found %s on Game State"), *GetNameSafe(this));
	}
}

bool UTilemapStateComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (bLoadTilemapFinished == false)
	{
		OutReason = TEXT("Tilemap Asset still loading.");
		return true;
	}
	return false;
}

void UTilemapStateComponent::OnExperienceLoaded(const UGridExperienceDefinition* Experience)
{
	// 首先尝试从场景中获取Tilemap Actor
	TArray<AActor*> OuterActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATilemap3DActor::StaticClass(), OuterActors);

	if (OuterActors.Num() >= 1)
	{
		UE_LOG(LogGrid, Verbose, TEXT("Multiple TilemapActors found in the scene, prioritizing the first TilemapActor."));
		TilemapActor = Cast<ATilemap3DActor>(OuterActors[0]);
	}else
	{
		// 当前场景中没有手动放置任何 TilemapActor
		// 尝试自动创建一个 Actor 并放置到场景中
		TilemapActor = GetWorld()->SpawnActor<ATilemap3DActor>();
		TilemapActor->SetActorTransform(FTransform::Identity);
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UTilemapStateComponent::LoadTilemapFinished_Step1));
}

void UTilemapStateComponent::LoadTilemapFinished_Step1()
{
	const UGridExperienceManagerComponent* ExperienceManagerComponent = FIND_STATE_COMP_IN_STATE(AGridGameState, GridExperienceManagerComponent);
	if (ExperienceManagerComponent == nullptr)
		return;
	if (ExperienceManagerComponent->GetCurrentExperienceChecked()->TilemapAsset == nullptr)
		return;
	// 尝试获取默认玩家控制对象
	const UTilemapAsset* TilemapAsset = ExperienceManagerComponent->GetCurrentExperienceChecked()->TilemapAsset;
	const APlayerController* DefaultPlayerController = GetWorld()->GetFirstPlayerController();
	APawn* Character = DefaultPlayerController->GetPawn();
	Character->SetActorTransform(TilemapAsset->PlayerStart);

	// 为TilemapActor设置TilemapAsset
	TilemapActor->SetupTilemapAsset(TilemapAsset);

	// 设置 Tilemap 数据到主相机
	UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Character);
	if (TilemapExtensionComponent)
	{
		TilemapExtensionComponent->SetTilemap(TilemapActor);
	}

	if (TilemapActor && TilemapExtensionComponent)
	{
		TilemapActor->ChessArray.Empty();
		TilemapActor->ChessArray.SetNumZeroed(TilemapExtensionComponent->GetPathfindingArrayNum());
	}

	// 放置棋子
	for (int32 PathfindingIndex = 0; PathfindingIndex < TilemapAsset->PathFindingBlocks.Num(); ++PathfindingIndex)
	{
		const int32 Index = TilemapAsset->PathFindingBlockToBlock(PathfindingIndex);
		if (!TilemapAsset->Blocks.IsValidIndex(Index))
			continue;
		
		const auto& Block = TilemapAsset->Blocks[Index];
		if (Block->ChessData == nullptr)
			continue;

		// 创建棋子数据
		const UGridAssetManager& AssetManager = UGridAssetManager::Get();
		UGridHeroInfo* ChessInfo = UGridHeroInfo::CreateHeroInfo(AssetManager.GetHeroData(Block->ChessData->ChessID));
		AGridChessBase* Chess = UChessBlueprintLibrary::SpawnChessOnTilemap(GetWorld(), PathfindingIndex, Block->ChessData, ChessInfo, TilemapActor);

		TilemapActor->ChessArray[PathfindingIndex] = Chess;
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UTilemapStateComponent::LoadTilemapFinished_Step2));
}

void UTilemapStateComponent::LoadTilemapFinished_Step2()
{
	if (OnTilemapSpawnFinished.IsBound())
		OnTilemapSpawnFinished.Broadcast();
	OnTilemapSpawnFinished.Clear();
	bLoadTilemapFinished = true;
}

void UTilemapStateComponent::CallOrRegister_OnSpawnFinished(FSimpleMulticastDelegate::FDelegate&& Delegate)
{
	if (bLoadTilemapFinished)
	{
		Delegate.Execute();
	}else
	{
		OnTilemapSpawnFinished.Add(Delegate);
	}
}
