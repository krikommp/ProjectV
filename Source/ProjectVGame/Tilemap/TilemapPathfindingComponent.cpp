// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapPathfindingComponent.h"

#include "TilemapExtensionComponent.h"

UTilemapPathfindingComponent::UTilemapPathfindingComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;	
}


FPathFindingCached::FPathFindingCached()
	:CurrentSearchStep(0)
{
	CanMoveToArray.Empty();
	DelayedSpiltPathfindingList.Empty();
}

void UTilemapPathfindingComponent::BeginPlay()
{
	Super::BeginPlay();
}

TArray<int32> UTilemapPathfindingComponent::PathfindingSelf(int32 MoveRange, int32 MaxMoveRange, bool bExcludeFriendly,
	bool bContinueFromLastPathfinding, bool bShowStartIndex)
{
	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, GetPawn<APawn>());

	if (TilemapExtensionComponent == nullptr)
	{
		return TArray<int32>{};
	}

	return Pathfinding(TilemapExtensionComponent->GetPathfindingIndex(), MoveRange, MaxMoveRange, bExcludeFriendly, bContinueFromLastPathfinding, bShowStartIndex);
}

TArray<int32> UTilemapPathfindingComponent::Pathfinding(int32 StartIndex, int32 MoveRange, int32 MaxMoveRange, bool bExcludeFriendly,
                                                        bool bContinueFromLastPathfinding, bool bShowStartIndex)
{
	const APawn* Pawn = GetPawn<APawn>();
	const UTilemapExtensionComponent* TilemapExtensionComponent = FIND_PAWN_COMP(TilemapExtensionComponent, Pawn);
	if (TilemapExtensionComponent == nullptr)
		return TArray<int32>{};
	
	TArray<int32> OutPathfindingArray;

	if (!bContinueFromLastPathfinding)
	{
		// 初始化
		PathFindingCached = {};
		PathFindingCached.CanMoveToArray.SetNum(TilemapExtensionComponent->GetPathfindingArrayNum());
	}

	// 寻路流程
	TArray<FTilemapPathFinding> OpenList;
	TArray<FTilemapPathFinding> OpenListChildren;
	TArray<FTilemapPathFinding> DelaySearchList;

	int32 CurrentIndex = StartIndex;
	MaxMoveRange = FMath::Max(MoveRange, MaxMoveRange);
	int32 PathfindingMove = MoveRange;

	if (bContinueFromLastPathfinding)
	{
		PathfindingMove = PathFindingCached.CurrentSearchStep + MoveRange;
		for (int32 Index = 0; Index < PathFindingCached.DelayedSpiltPathfindingList.Num();)
		{
			if (PathFindingCached.DelayedSpiltPathfindingList[Index].Cost <= MaxMoveRange)
			{
				OpenList.Add({
					PathFindingCached.DelayedSpiltPathfindingList[Index].Index, PathFindingCached.DelayedSpiltPathfindingList[Index].Cost,
					PathFindingCached.DelayedSpiltPathfindingList[Index].Parent
				});
				PathFindingCached.CanMoveToArray[PathFindingCached.DelayedSpiltPathfindingList[Index].Index].Index = 0;
				PathFindingCached.CanMoveToArray[PathFindingCached.DelayedSpiltPathfindingList[Index].Index].Cost = PathFindingCached.DelayedSpiltPathfindingList[Index].
					Cost;
				PathFindingCached.CanMoveToArray[PathFindingCached.DelayedSpiltPathfindingList[Index].Index].Parent = PathFindingCached.DelayedSpiltPathfindingList[Index].
					Parent;
				PathFindingCached.DelayedSpiltPathfindingList.RemoveAt(Index);
			}
			else
			{
				++Index;
			}
		}
	}else
	{
		// 当需要寻路的范围过小，那么就直接返回当前位置
		if (PathfindingMove <= 0)
		{
			PathFindingCached.CanMoveToArray[CurrentIndex] = { CurrentIndex, 0, CurrentIndex };
			return OutPathfindingArray;
		}
		// 把当前位置加入到 OpenList
		OpenList.Add({ CurrentIndex, 0, CurrentIndex });
	}
	for (int32 SearchStep = PathFindingCached.CurrentSearchStep; SearchStep < PathfindingMove; ++SearchStep)
	{
		for (const auto& OpenListElem : OpenList)
		{
			// 判断这个Tile的移动消耗是否满足当前SearchStep,
			if (SearchStep == OpenListElem.Cost)
			{
				const FTilemapPathFindingBlock& PathfindingBlock = TilemapExtensionComponent->GetPathfindingBlock(OpenListElem.Index);
				// 遍历这个 Block 的所有相邻节点
				for (const auto& Edge : PathfindingBlock.EdgeArray)
				{
					// 通过 CanMoveToArray 来判断是否搜索到该节点
					if (PathFindingCached.CanMoveToArray[Edge.Index].Cost == INDEX_NONE)
					{
						// 未搜索到逻辑
						// 判断该点上是否存在棋子
						if (TilemapExtensionComponent->CheckChessExist(Edge.Index))
						{
							// todo... 存在棋子
						}else
						{
							// 如果在这次寻路范围内
							if (Edge.Cost + OpenListElem.Cost <= MaxMoveRange)
							{
								OpenListChildren.Add({ Edge.Index, Edge.Cost + OpenListElem.Cost, OpenListElem.Index });
								PathFindingCached.CanMoveToArray[Edge.Index] = { 0, Edge.Cost + OpenListElem.Cost, OpenListElem.Index };
							}else
							{
								// 如果可以在下次寻路中找到
								if (Edge.Cost <= MaxMoveRange)
								{
									PathFindingCached.DelayedSpiltPathfindingList.Add({ Edge.Index, Edge.Cost + OpenListElem.Cost, OpenListElem.Index });
								}
							}
						}
					}
				}
			}else
			{
				// 将他加入到下一次寻路中
				DelaySearchList.Add({ OpenListElem.Index, OpenListElem.Cost, OpenListElem.Parent });
			}
		}
		
		// 完成本层寻路
		OpenList.Empty();
		// 执行下一次搜索
		PathFindingCached.CurrentSearchStep += 1;
		OpenList.Append(OpenListChildren);
		OpenList.Append(DelaySearchList);
		// 判断是终止
		if (OpenList.IsEmpty())
		{
			break;
		}
		if (PathFindingCached.CurrentSearchStep <= PathfindingMove)
		{
			break;
		}
	}
	
	PathFindingCached.CanMoveToArray[CurrentIndex] = { CurrentIndex, 0, CurrentIndex };
	for (int32 Index = 0; Index < PathFindingCached.CanMoveToArray.Num(); ++Index)
	{
		if (PathFindingCached.CanMoveToArray[Index].Cost == INDEX_NONE)
			continue;
		OutPathfindingArray.Add(Index);
	}
	
	return OutPathfindingArray;
}
