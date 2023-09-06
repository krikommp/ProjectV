#pragma once

class ATilemap3DActor;
class AGridChessBase;
class UGridChessData;
class UGridHeroInfo;

struct PROJECTVGAME_API FTilemapSpawnParameters
{
	// 目标
	AGridChessBase* Chess;
	// Chess 数据
	UGridChessData* ChessData;
	// Chess 信息
	UGridHeroInfo* ChessInfo;
	// Tilemap 数据
	ATilemap3DActor* Tilemap;
	// 寻路索引
	int32 PathfindingIndex;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTilemapSpawnChess, const FTilemapSpawnParameters& /* SpawnParameters */)
extern FOnTilemapSpawnChess GOnTilemapSpawnChess;