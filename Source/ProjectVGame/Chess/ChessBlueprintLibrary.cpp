// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBlueprintLibrary.h"

#include "GridChessBase.h"
#include "GridChessData.h"
#include "Heros/GridHeroData.h"
#include "Heros/GridHeroInfo.h"
#include "Tilemap/TilemapExtensionComponent.h"
#include "Tilemap/TilemapSpawnParameters.h"

FOnTilemapSpawnChess GOnTilemapSpawnChess;

AGridChessBase* UChessBlueprintLibrary::SpawnChessOnTilemapByCursor(const UObject* WorldContextObject,
                                                                    ETraceTypeQuery TraceChannel,
                                                                    UGridHeroInfo* ChessInfo)
{
	const APlayerController* PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(
		TilemapExtensionComponent, PlayerController->GetPawn());
	if (TilemapExtensionComponent == nullptr)
		return nullptr;

	int32 PathfindingIndex;
	FVector Location;
	TilemapExtensionComponent->GetHitLocationAndIndex(TraceChannel, PathfindingIndex, Location);

	if (!TilemapExtensionComponent->GetTilemap()->PathFindingBlocks.IsValidIndex(PathfindingIndex))
		return nullptr;

	if (CheckIndexInPlayerStartRange(WorldContextObject, PathfindingIndex))
		return nullptr;

	const auto& PathfindingBlock = TilemapExtensionComponent->GetTilemap()->PathFindingBlocks[PathfindingIndex];
	UGridChessData* NewChessData = NewObject<UGridChessData>();
	NewChessData->ChessClass = AGridChessBase::StaticClass();
	NewChessData->ChessTransform = FTransform(PathfindingBlock.Location);
	NewChessData->PlayerIndex = 0;
	NewChessData->ChessID = ChessInfo->HeroData.HeroID;
	NewChessData->Team = ETeamType::Player;

	return SpawnChessOnTilemap(WorldContextObject, PathfindingIndex, NewChessData, ChessInfo,
	                           TilemapExtensionComponent->GetTilemapActor());
}

bool UChessBlueprintLibrary::CheckIndexInPlayerStartRange(const UObject* WorldContextObject, const int32 PathfindingIndex)
{
	const APlayerController* PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(
		TilemapExtensionComponent, PlayerController->GetPawn());
	if (TilemapExtensionComponent == nullptr)
		return false;
	
	return TilemapExtensionComponent->CheckIndexInPlayerStartRange(PathfindingIndex);
}

AGridChessBase* UChessBlueprintLibrary::SpawnChessOnTilemap(const UObject* WorldContextObject, int32 PathfindingIndex,
                                                            UGridChessData* ChessData, UGridHeroInfo* ChessInfo,
                                                            ATilemap3DActor* Tilemap3DActor)
{
	const FActorSpawnParameters Parameters;
	AGridChessBase* Chess = WorldContextObject->GetWorld()->SpawnActor<AGridChessBase>(
		ChessData->ChessClass, ChessData->ChessTransform, Parameters);
	if (Chess == nullptr)
		return nullptr;

	// 创建棋子放置事件消息
	FTilemapSpawnParameters SpawnParameters;
	SpawnParameters.Chess = Chess;
	SpawnParameters.Tilemap = Tilemap3DActor;
	SpawnParameters.ChessData = ChessData;
	SpawnParameters.ChessInfo = ChessInfo;
	SpawnParameters.PathfindingIndex = PathfindingIndex;

	GOnTilemapSpawnChess.Broadcast(SpawnParameters);

	return Chess;
}
