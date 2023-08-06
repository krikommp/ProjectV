// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "TilemapPathFinding.h"
#include "TilemapPathfindingComponent.generated.h"

/**
 * FPathFindingCached
 *	
 * 用于缓存寻路中的数据
 */
struct FPathFindingCached
{
	// 记录当前可达的路径
	TArray<FTilemapPathFinding> CanMoveToArray;
	// 当前搜索深度
	int32 CurrentSearchStep = 0;
	// 需要等待下一次再执行的寻路
	TArray<FTilemapPathFinding> DelayedSpiltPathfindingList;
	// 路径点
	TArray<int32> PathIndexArray;

	FPathFindingCached();
};

/**
 * UTilemapPathfindingComponent
 *
 * Tilemap 寻路组件，挂载到需要执行寻路的Pawn上
 * 依赖 TilemapExtensionComponent
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UTilemapPathfindingComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UTilemapPathfindingComponent* FindTilemapPathfindingComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UTilemapPathfindingComponent>() : nullptr);
	}

protected:
	virtual void BeginPlay() override;

public:
	//根据输入的当前位置和移动范围大小，计算出可移动到的Tiles
	UFUNCTION(BlueprintCallable)
	TArray<int32> Pathfinding(int32 StartIndex, int32 MoveRange, int32 MaxMoveRange, bool bExcludeFriendly,
	                          bool bContinueFromLastPathfinding, bool bShowStartIndex = false);

	// 自身寻路
	// 根据输入的当前位置和移动范围大小，计算出可移动到的Tiles
	UFUNCTION(BlueprintCallable)
	TArray<int32> PathfindingSelf(int32 MoveRange, int32 MaxMoveRange, bool bExcludeFriendly,
							  bool bContinueFromLastPathfinding, bool bShowStartIndex = false);

	// 判断传入的位置是否在寻路范围内
	UFUNCTION(BlueprintPure)
	bool CheckWithinPathfinding(int32 Index);

	// 计算从当前位置到目标位置的路径
	UFUNCTION(BlueprintCallable)
	const TArray<int32>& FindPathToIndex(int32 EndIndex, int32 StopFromTarget, bool& bUpdate);

	// 清空寻路缓存
	UFUNCTION(BlueprintCallable)
	void ClearFindPath();

private:
	FPathFindingCached PathFindingCached;
};
