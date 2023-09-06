// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBlueprintLibrary.h"

#include "GridChessBase.h"
#include "GridChessData.h"
#include "Heros/GridHeroData.h"
#include "Heros/GridHeroInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Team/ChessTeamComponent.h"
#include "Tilemap/TilemapExtensionComponent.h"
#include "Tilemap/TilemapSpawnParameters.h"

FOnTilemapSpawnChess GOnTilemapSpawnChess;

AGridChessBase* UChessBlueprintLibrary::SpawnChessOnTilemapByCursor(const UObject* WorldContextObject,
                                                                    ETraceTypeQuery TraceChannel,
                                                                    UGridHeroInfo* ChessInfo)
{
	const APlayerController* PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, PlayerController->GetPawn());
	if (TilemapExtensionComponent == nullptr)
		return nullptr;

	int32 PathfindingIndex;
	FVector Location;
	TilemapExtensionComponent->GetHitLocationAndIndex(TraceChannel, PathfindingIndex, Location);

	if (!TilemapExtensionComponent->GetTilemap()->PathFindingBlocks.IsValidIndex(PathfindingIndex))
		return nullptr;

	if (!CheckIndexInPlayerStartRange(WorldContextObject, PathfindingIndex))
		return nullptr;

	const auto& PathfindingBlock = TilemapExtensionComponent->GetTilemap()->PathFindingBlocks[PathfindingIndex];
	UGridChessData* NewChessData = NewObject<UGridChessData>();
	NewChessData->ChessTransform = FTransform(PathfindingBlock.Location);
	NewChessData->PlayerIndex = 0; 
	NewChessData->ChessID = ChessInfo->HeroData.HeroID;
	NewChessData->Team = ETeamType::Player;

	return SpawnChessOnTilemap(WorldContextObject, PathfindingIndex, NewChessData, ChessInfo,
	                           TilemapExtensionComponent->GetTilemapActor());
}

void UChessBlueprintLibrary::DestroyChessOnTilemap(const UObject* WorldContextObject, AGridChessBase* Chess,
	const ETeamType TeamType)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UChessTeamComponent* TeamComponent = World->GetGameState()->FindComponentByClass<UChessTeamComponent>())
		{
			TeamComponent->RemoveTeamMember(TeamType, Chess);
		}
		World->DestroyActor(Chess);
	}
}

bool UChessBlueprintLibrary::CheckIndexInPlayerStartRange(const UObject* WorldContextObject,
                                                          const int32 PathfindingIndex)
{
	const APlayerController* PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(
		TilemapExtensionComponent, PlayerController->GetPawn());
	if (TilemapExtensionComponent == nullptr)
		return false;

	return TilemapExtensionComponent->CheckIndexInPlayerStartRange(PathfindingIndex);
}

TArray<FTilemapPathFindingBlock> UChessBlueprintLibrary::GetPlayerStartLocations(const UObject* WorldContextObject)
{
	const APlayerController* PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController();

	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(
		TilemapExtensionComponent, PlayerController->GetPawn());
	if (TilemapExtensionComponent == nullptr)
		return {};

	TArray<FTilemapPathFindingBlock> Locations;

	for (int32 PathfindingBlockIndex = 0; PathfindingBlockIndex < TilemapExtensionComponent->GetTilemap()->
	     PathFindingBlocks.Num(); ++PathfindingBlockIndex)
	{
		UBlock* Block = TilemapExtensionComponent->GetBlockFromPathfindingIndex(PathfindingBlockIndex);
		if (Block == nullptr)
			continue;
		if (Block->bPlayerChessStart == false)
			continue;

		Locations.Add(TilemapExtensionComponent->GetTilemap()->PathFindingBlocks[PathfindingBlockIndex]);
	}

	return Locations;
}

UDecalComponent* UChessBlueprintLibrary::SpawnDecalOnTilemap(const UObject* WorldContextObject, const FVector& Location,
                                                             UMaterialInterface* Mat, const FLinearColor& Color)
{
	UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(Mat, WorldContextObject->GetWorld());
	if (DynamicMat == nullptr)
		return nullptr;
	DynamicMat->SetVectorParameterValue("Color", Color);
	static FVector DecalSize = {95.0f, 45.0f, 45.0f};
	static FRotator DecalRotator = {90.0f, 0.0f, 0.0f};
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(WorldContextObject, DynamicMat,
	                                                                         DecalSize, Location, DecalRotator);

	return DecalComponent;
}

AGridChessBase* UChessBlueprintLibrary::SpawnChessOnTilemap(const UObject* WorldContextObject, int32 PathfindingIndex,
                                                            UGridChessData* ChessData, UGridHeroInfo* ChessInfo,
                                                            ATilemap3DActor* Tilemap3DActor)
{
	const FActorSpawnParameters Parameters;
	AGridChessBase* Chess = WorldContextObject->GetWorld()->SpawnActor<AGridChessBase>(
		ChessInfo->HeroData.ChessClass, ChessData->ChessTransform, Parameters);
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
