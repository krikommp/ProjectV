// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapManager.h"

#include "GridGameplayTags.h"
#include "GridLogChannel.h"
#include "GridMapFunctionLibrary.h"
#include "GridTraceChannel.h"
#include "ChessPieces/GridChessPiece.h"
#include "ChessPieces/GridChessPieceExtensionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "System/GridAssetManager.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGridMapManager::AGridMapManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	CollisionPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionPlane"));
	CollisionPlane->SetupAttachment(RootComponent);
	Heightmap = CreateDefaultSubobject<UBoxComponent>(TEXT("HeightMap"));
	Heightmap->SetupAttachment(RootComponent);
	DefaultTileInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("DefaultTileInstance"));
	DefaultTileInstance->SetupAttachment(RootComponent);

	RootComponent->SetMobility(EComponentMobility::Static);
	CollisionPlane->SetMobility(EComponentMobility::Static);
	Heightmap->SetMobility(EComponentMobility::Static);

	DisplayIndexesTextRenderArray.Empty();

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AGridMapManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

FVector AGridMapManager::IndexToVectorOnGrid(int32 InIndex, float ZOffset)
{
	return GetActorLocation() + VectorFieldArray[InIndex] + FVector(0.0f, 0.0f, ZOffset);
}

void AGridMapManager::BeginPlay()
{
	Super::BeginPlay();
}

TArray<int32> AGridMapManager::PathFinding(int32 InStartIndex, int32 InMoveRange, int32 InMaxMoveRange,
                                           bool bExcludeFriendly,
                                           bool bContinueFromLastPathfinding, bool bShowStartIndex)
{
	TArray<FStructPathFinding> DelayedSpiltPathfindingList;
	TArray<FStructPathFinding> OpenList;
	TArray<FStructPathFinding> OpenListChildren;
	TArray<FStructPathFinding> DelaySearchList;

	TArray<int32> OutTileIndex;
	CurrentIndex = InStartIndex;
	MaxMove = FMath::Max(InMoveRange, InMaxMoveRange);
	int32 PathfindingMove = InMoveRange;
	if (bContinueFromLastPathfinding)
	{
		PathfindingMove = CurrentSearchStep + InMoveRange;
		for (int32 Index = 0; Index < DelayedSpiltPathfindingList.Num();)
		{
			if (DelayedSpiltPathfindingList[Index].Cost <= MaxMove)
			{
				OpenList.Add({
					DelayedSpiltPathfindingList[Index].Index, DelayedSpiltPathfindingList[Index].Cost,
					DelayedSpiltPathfindingList[Index].Parent
				});
				CanMoveToArray[DelayedSpiltPathfindingList[Index].Index].Index = 0;
				CanMoveToArray[DelayedSpiltPathfindingList[Index].Index].Cost = DelayedSpiltPathfindingList[Index].Cost;
				CanMoveToArray[DelayedSpiltPathfindingList[Index].Index].Parent = DelayedSpiltPathfindingList[Index].
					Parent;
				DelayedSpiltPathfindingList.RemoveAt(Index);
			}
			else
			{
				++Index;
			}
		}
	}
	else
	{
		DelayedSpiltPathfindingList.Empty();
		ResetPathfindingArraysAndClearMeshes();
		if (PathfindingMove <= 0)
		{
			CanMoveToArray[CurrentIndex] = {CurrentIndex, 0, CurrentIndex};
			IndexCanMoveToArray.Add({CurrentIndex, 0, CurrentIndex});
			return OutTileIndex;
		}
		OpenList.Add({CurrentIndex, 0, CurrentIndex});
	}
	// todo.. pathfinding type, current is standard
	for (int32 SearchStep = CurrentSearchStep; SearchStep < PathfindingMove; ++SearchStep)
	{
		SearchAndAddAdjacentTiles(DelayedSpiltPathfindingList, OpenList, OpenListChildren, DelaySearchList,
		                          bShowStartIndex, InStartIndex, bExcludeFriendly);
		// 完成上一步寻路
		OpenList.Empty();
		IndexCanMoveToArray.Append(OpenListChildren);
		// 移动到下一步寻路
		CurrentSearchStep += 1;
		// 将待寻路Tile加入到OpenList中
		OpenList.Append(OpenListChildren);
		OpenList.Append(DelaySearchList);
		OpenListChildren.Empty();
		DelaySearchList.Empty();
		// 判断是否终止搜索
		if (OpenList.IsEmpty())
		{
			break;
		}
		if (CurrentSearchStep >= PathfindingMove)
		{
			break;
		}
	}
	// 结束寻路
	CanMoveToArray[CurrentIndex] = {CurrentIndex, 0, CurrentIndex};
	IndexCanMoveToArray.Add({CurrentIndex, 0, CurrentIndex});
	// todo... for path finding friendly
	for (const auto& CanMove : IndexCanMoveToArray)
	{
		if (bShowStartIndex && CanMove.Index == InStartIndex) continue;
		OutTileIndex.Add(CanMove.Index);
	}
	return OutTileIndex;
}

const TArray<int32>& AGridMapManager::FindTilesInRange(int32 StartIndex, int32 InRange, bool bCheckVisibility,
                                                       bool bFindOnlyPawns,
                                                       float MaxZDifference, int32 MinimumRange,
                                                       bool bExcludeFriendly)
{
	DiscoverableTileIndexArray.Empty();
	// 根据当前位置和查找范围，生成范围数组
	GetIndexesInRange(StartIndex, InRange);

	// 清理数据
	RangeArray.Empty();
	RangeArray.SetNumZeroed(GridSizeX * GridSizeY * GridSizeZ);
	TilesInSightArray.Empty();

	// 对每个范围内的Tile进行可见性测试
	for (const auto& TileInRange : TilesInRangeArray)
	{
		CheckIfTileIsVisibleFromOtherTile(StartIndex, TileInRange.Index, bFindOnlyPawns, MaxZDifference,
		                                  TileInRange.Distance, MinimumRange, bCheckVisibility, bExcludeFriendly);
	}
	DiscoverableTileIndexArray.Append(TilesInSightArray);
	return DiscoverableTileIndexArray;
}

TArray<int32> AGridMapManager::FindChessPieceInRange(const TArray<FStructRange>& InTileInRangeArray,
                                                     AGridChessPiece* InstigatorChessPiece, int32 Distance)
{
	TArray<int32> OutTileIndexArray;
	for (const auto& TileInRange : InTileInRangeArray)
	{
		// 如果开始位置不可逾越的，那么跳过
		if (IsTileImpassable(TileInRange.Index)) continue;

		// 如果距离过大，也跳过
		if (TileInRange.Distance > Distance) continue;

		//if (!PawnArray.IsValidIndex(TileInRange.Index) || PawnArray[TileInRange.Index] == nullptr) continue;

		if (PawnArray.IsValidIndex(TileInRange.Index) && PawnArray[TileInRange.Index] == InstigatorChessPiece) continue;

		OutTileIndexArray.Add(TileInRange.Index);
	}
	return OutTileIndexArray;
}

bool AGridMapManager::FindPathWithinPathfindingArray(int32 IndexPathEnd, bool bCreatePathSpline,
                                                     bool bDisplayPath,
                                                     bool bDisplayPathAsSpline, int32 StopFromTarget)
{
	const TArray<int32>& LocalPathIndexArray = FindPathToIndex(CanMoveToArray, IndexPathEnd, StopFromTarget);
	if (bCreatePathSpline)
	{
		CreateSplinePath(LocalPathIndexArray);
	}
	if (bDisplayPath)
	{
		if (bDisplayPathAsSpline)
		{
			DisplayPathAsSpline();
		}
		else
		{
			UE_LOG(LogGrid, Error,
			       TEXT("[AGridMapManager::FindPathWithinPathfindingArray]: DisplayPathAsInstancedMeshes."));
		}
	}
	return true;
}

int32 AGridMapManager::FindDistanceInTilesBetweenIndexes(int32 Index1, int32 Index2)
{
	int32 Distance = 0;

	const int32 XYPlane1 = Index1 % (GridSizeX * GridSizeY);
	const int32 XYPlane2 = Index2 % (GridSizeX * GridSizeY);
	const int32 XPos1 = XYPlane1 % GridSizeX;
	const int32 XPos2 = XYPlane2 % GridSizeX;
	const int32 YPos1 = XYPlane1 / GridSizeX;
	const int32 YPos2 = XYPlane2 / GridSizeX;

	const int32 XOffset = FMath::Abs(XPos1 - XPos2);
	const int32 YOffset = FMath::Abs(YPos1 - YPos2);

	Distance = XOffset + YOffset;

	return Distance;
}

void AGridMapManager::DisplayRangeMarkers(const TArray<int32>& InTileIndexArray, UMaterialInterface* DecalMaterial,
                                          bool bOverrider)
{
	for (const auto& TileIndex : InTileIndexArray)
	{
		if (VectorFieldArray.IsValidIndex(TileIndex))
		{
			const FVector TileLocation = VectorFieldArray[TileIndex] + GetActorLocation();
			if (bOverrider)
			{
				TObjectPtr<UDecalComponent>* FoundDecalComponent = CurrentDecalsArray.FindByPredicate(
					[&TileLocation](const UDecalComponent* DecalComponent)
					{
						return UKismetMathLibrary::EqualEqual_VectorVector(
							DecalComponent->GetComponentLocation(), TileLocation);
					});
				if (FoundDecalComponent)
				{
					FoundDecalComponent->Get()->SetDecalMaterial(DecalMaterial);
					continue;
				}
			}
			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(), DecalMaterial, DecalSizeSquare, TileLocation, {90.0f, 0.0f, 0.0f});
			CurrentDecalsArray.Add(DecalComponent);
		}
	}
}

bool AGridMapManager::IsTileImpassable(int32 Index)
{
	if (EdgeArrayInteger.IsValidIndex(Index))
	{
		return EdgeArrayInteger[Index].Index.Num() == 0;
	}
	return true;
}

bool AGridMapManager::GetHitLocationAndIndex(APlayerController* InController, int32& OutIndex, FVector& OutLocation)
{
	FHitResult HitResult;
	InController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(PathTrace), false, HitResult);
	if (HitResult.IsValidBlockingHit())
	{
		OutIndex = UGridMapFunctionLibrary::VectorToIndex3D(this, HitResult.Location);
		OutLocation = HitResult.Location;
	}
	if (OutIndex <= INDEX_NONE || IsTileImpassable(OutIndex))
	{
		return false;
	}
	return HitResult.IsValidBlockingHit();
}

TArray<int32> AGridMapManager::K2_GetIndexCanMove() const
{
	TArray<int32> OutIndexArray;
	for (const auto& CanMove : IndexCanMoveToArray)
	{
		OutIndexArray.Add(CanMove.Index);
	}
	return OutIndexArray;
}

void AGridMapManager::SearchAndAddAdjacentTiles(TArray<FStructPathFinding>& InDelayedSpiltPathfindingList,
                                                TArray<FStructPathFinding>& InOpenList,
                                                TArray<FStructPathFinding>& InOpenListChildren,
                                                TArray<FStructPathFinding>& InDelaySearchList
                                                , bool bShowStartIndex, int32 StartIndex, bool bExcludeFriendly)
{
	for (const auto& OpenListElem : InOpenList)
	{
		// 判断这个Tile的移动消耗是否满足当前SearchStep,如果不是，就将他加入到下一次寻路中
		if (CurrentSearchStep == OpenListElem.Cost)
		{
			// 获取该Tile所有相邻的Edge，将他们加入到寻路搜索路径中
			for (const auto& Edge : EdgeArray[OpenListElem.Index].Array)
			{
				// 判断这个Edge是否已经被搜索到，如果搜索到，那么对应cost不为0
				if (CanMoveToArray[Edge.Index].Cost == 0)
				{
					// 接着判断在这个位置上，是否有棋子
					if (!PawnArray.IsEmpty() && PawnArray[Edge.Index] != nullptr)
					{
						if (Edge.Index == StartIndex && !bShowStartIndex)
						{
							//OpenListChildren.Add({Edge.Index, Edge.Cost + OpenListElem.Cost, OpenListElem.Index});
							CanMoveToArray[Edge.Index] = {0, Edge.Cost + OpenListElem.Cost, OpenListElem.Index};
						}
						else
						{
							// 如果有，就将棋子加入到ReachablePawnsArray，方便AI查询
							// 同时判断是否为同盟
							if (bExcludeFriendly && PawnArray[StartIndex] != nullptr)
							{
								const AGridChessPiece* LHS = PawnArray[StartIndex];
								const AGridChessPiece* RHS = PawnArray[Edge.Index];
								if (const UGridChessPieceExtensionComponent* ChessPieceExtComp =
									UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(LHS))
								{
									if (!ChessPieceExtComp->IsFaction(RHS))
									{
										ReachablePawnsArray.Add(Edge.Index);
									}
								}
							}
							else
							{
								ReachablePawnsArray.Add(Edge.Index);
							}
							//OpenListChildren.Add({Edge.Index, Edge.Cost + OpenListElem.Cost, OpenListElem.Index});
							CanMoveToArray[Edge.Index] = {0, Edge.Cost + OpenListElem.Cost, OpenListElem.Index};
						}
					}
					else
					{
						// 否则就尝试加入到下次搜索
						if ((Edge.Cost + OpenListElem.Cost) <= MaxMove)
						{
							InOpenListChildren.Add({Edge.Index, Edge.Cost + OpenListElem.Cost, OpenListElem.Index});
							CanMoveToArray[Edge.Index] = {0, Edge.Cost + OpenListElem.Cost, OpenListElem.Index};
						}
						else
						{
							if (Edge.Cost <= MaxMove)
							{
								InDelayedSpiltPathfindingList.Add({
									Edge.Index, Edge.Cost + OpenListElem.Cost, OpenListElem.Index
								});
							}
						}
					}
				}
			}
		}
		else
		{
			InDelaySearchList.Add({OpenListElem.Index, OpenListElem.Cost, OpenListElem.Parent});
		}
	}
}

const TArray<int32>& AGridMapManager::FindPathToIndex(const TArray<FStructPathFinding>& InCanMoveToArray,
                                                      int32 InEndIndex, int32 StopFromTarget)
{
	PathIndexArray.Empty();
	int32 PathHolderTemp = InEndIndex;
	int32 LocalDebugCounter = 0;
	int32 TempStopFromTarget = StopFromTarget;
	if (TempStopFromTarget == 0)
		PathIndexArray.Add(PathHolderTemp);
	else
		--TempStopFromTarget;
	while (true)
	{
		if ((++LocalDebugCounter) > 1000)
		{
			UE_LOG(LogGrid, Error, TEXT("[AGridMapManager::FindPathToIndex]: Path Too Long!!"));
			return PathIndexArray;
		}
		const auto& ParentTile = InCanMoveToArray[PathHolderTemp];
		if (ParentTile.Cost == 0)
		{
			return PathIndexArray;
		}
		PathHolderTemp = ParentTile.Parent;
		if (TempStopFromTarget != 0)
			--TempStopFromTarget;
		else
			PathIndexArray.Add(PathHolderTemp);
	}
}

void AGridMapManager::GetIndexesInRange(int32 StartIndex, int32 Range)
{
	const int32 CurrentRangeIndex = StartIndex % (GridSizeX * GridSizeY);
	const int32 CurrentRange = Range;

	// 检查当前Pawn所在的Tile是否靠近网格边缘
	// 因为我们需要在Pawn的Range范围查找，为了避免Pawn过于靠近边缘导致加上Range变量后索引超出网格范围
	// 所以我们在这里进行一些边缘变量计算，找到Pawn合适的Range

	// 将当前Pawn所处的Tile和地图左侧的距离与Range进行比较
	const int32 RangeCheckWest = FMath::Min(CurrentRangeIndex % GridSizeX, CurrentRange);
	// 将当前Pawn所处的Tile和地图右侧的距离与Range进行比较
	const int32 RangeCheckEast = FMath::Min(GridSizeX - (CurrentRangeIndex % GridSizeX) - 1,
	                                        CurrentRange);
	// 将当前Pawn所处的Tile和地图下方的距离与Range进行比较
	const int32 RangeCheckNorth = FMath::Min(CurrentRangeIndex / GridSizeX, CurrentRange);
	// 将当前Pawn所处的Tile和地图上方的距离与Range进行比较
	const int32 RangeCheckSouth = FMath::Min(
		((GridSizeY - 1) - ((CurrentRangeIndex % (GridSizeX * GridSizeY)) /
			GridSizeX)), CurrentRange);

	// 在添加前清空数组
	TilesInRangeArray.Empty();

	// 计算高度的影响
	for (int32 Index = 0; Index < GridSizeZ; ++Index)
	{
		const int32 CurrentRangeIndexHeightMap = (Index * GridSizeX * GridSizeY) + (CurrentRangeIndex % (GridSizeX *
			GridSizeY));
		if (IsDiagonalMovement())
		{
			for (int32 Vertical = RangeCheckNorth * -1; Vertical <= RangeCheckSouth; ++Vertical)
			{
				for (int32 Horizontal = RangeCheckWest * -1; Horizontal <= RangeCheckEast; ++Horizontal)
				{
					const int32 CheckIndex = Vertical * GridSizeX + CurrentRangeIndexHeightMap + Horizontal;
					if (CheckIndex >= 0 && CheckIndex <= (GridSizeX * GridSizeY * GridSizeZ))
					{
						const int32 Distance = FMath::Max(FMath::Abs(Vertical), FMath::Abs(Horizontal));
						TilesInRangeArray.Add(FStructRange(CheckIndex, Distance));
					}
				}
			}
		}
		else
		{
			for (int32 Vertical = RangeCheckNorth * -1; Vertical <= RangeCheckSouth; ++Vertical)
			{
				int32 A = FMath::Max(CurrentRange - RangeCheckWest - FMath::Abs(Vertical), 0.0f) + (RangeCheckWest * -1
					+ FMath::Abs(Vertical) - (CurrentRange - RangeCheckWest));
				int32 B = (CurrentRangeIndexHeightMap / GridSizeX % 2 - 1 + FMath::Abs(Vertical) * -1) / 2 +
					RangeCheckEast + (CurrentRange - RangeCheckEast) - FMath::Max(
						CurrentRange - RangeCheckEast - FMath::Abs(Vertical), 0);
				for (int32 Horizontal = A; Horizontal <= B; ++Horizontal)
				{
					const int32 CheckIndex = GridSizeX * Vertical + CurrentRangeIndexHeightMap + Horizontal;
					if (CheckIndex >= 0 && CheckIndex <= (GridSizeX * GridSizeY * GridSizeZ))
					{
						const int32 Distance = FMath::Abs(Vertical) + FMath::Abs(Horizontal);
						TilesInRangeArray.Add(FStructRange(CheckIndex, Distance));
					}
				}
			}
		}
	}
}

void AGridMapManager::CheckIfTileIsVisibleFromOtherTile(int32 Index, int32 TargetIndex, bool bFindOnlyPawns,
                                                        float MaxZDifference, int32 Distance, int32 MinimumRange,
                                                        bool bCheckVisibility, bool bExcludeFriendly)
{
	// 如果开始位置不可逾越的，那么直接返回
	if (IsTileImpassable(Index)) return;

	if (bFindOnlyPawns)
	{
		// 如果目标位置上没有任何棋子单位，那么立即返回
		if (!PawnArray.IsValidIndex(TargetIndex) || PawnArray[TargetIndex] == nullptr)
		{
			return;
		}
	}

	// 判断是否需要排除友好棋子对象
	if (bExcludeFriendly)
	{
		if (PawnArray.IsValidIndex(TargetIndex) && PawnArray[TargetIndex] != nullptr)
		{
			if (UGridChessPieceExtensionComponent* ChessPieceExtComp =
				UGridChessPieceExtensionComponent::FindGridChessPieceExtensionComponent(PawnArray[TargetIndex]))
			{
				if (ChessPieceExtComp->IsFaction(PawnArray[Index])) return;
			}
		}
	}

	// 获取开始位置和目标位置，判断他们之间的高度差，如果差距过大则不可见
	const FVector CurrentVisibilityVector = IndexToVectorOnGrid(Index, VisibilityTraceHeight);
	const FVector TargetVisibilityVector = IndexToVectorOnGrid(TargetIndex, VisibilityTraceHeight);

	bool bHighUp = CurrentVisibilityVector.Z > TargetVisibilityVector.Z;
	if (!bHighUp && FMath::Abs(TargetVisibilityVector.Z - CurrentVisibilityVector.Z) >= MaxZDifference)
	{
		return;
	}

	// 如果可见范围过小，也不可见
	if (Distance <= MinimumRange)
	{
		return;
	}

	if (bCheckVisibility)
	{
		TArray<AActor*> IgnoreActors;
		FHitResult HitResult;
		if (bHighUp)
		{
			UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				FVector(TargetVisibilityVector.X, TargetVisibilityVector.Y, IndexToVectorOnGrid(Index, 0).Z),
				TargetVisibilityVector,
				UEngineTypes::ConvertToTraceType(RangeTrace),
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				HitResult,
				false);
			// 如果发生碰撞，说明该Tile无法通行
			if (HitResult.IsValidBlockingHit())
			{
				return;
			}
		}
		else
		{
			UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				CurrentVisibilityVector,
				TargetVisibilityVector,
				UEngineTypes::ConvertToTraceType(RangeTrace),
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				HitResult,
				false);
			// 如果发生碰撞，说明该Tile无法通行
			if (HitResult.IsValidBlockingHit())
			{
				return;
			}
		}
	}

	UGridMapFunctionLibrary::SetArrayElement(Distance, RangeArray, TargetIndex);
	TilesInSightArray.Add(TargetIndex);
}

void AGridMapManager::PathFinding_Internal(int32 InStartIndex, int32 InMoveRange, int32 InMaxMoveRange,
                                           bool bExcludeFriendly, bool bContinueFromLastPathfinding,
                                           bool bShowStartIndex, TArray<FStructPathFinding>& OutCanMoveToArray,
                                           TArray<FStructPathFinding>& OutIndexCanMoveToArray,
                                           TArray<int32>& OutTileIndexes)
{
	TArray<FStructPathFinding> DelayedSpiltPathfindingList;
	TArray<FStructPathFinding> OpenList;
	TArray<FStructPathFinding> OpenListChildren;
	TArray<FStructPathFinding> DelaySearchList;

	CurrentIndex = InStartIndex;
	MaxMove = FMath::Max(InMoveRange, InMaxMoveRange);
	int32 PathfindingMove = InMoveRange;
	if (bContinueFromLastPathfinding)
	{
		PathfindingMove = CurrentSearchStep + InMoveRange;
		for (int32 Index = 0; Index < DelayedSpiltPathfindingList.Num();)
		{
			if (DelayedSpiltPathfindingList[Index].Cost <= MaxMove)
			{
				OpenList.Add({
					DelayedSpiltPathfindingList[Index].Index, DelayedSpiltPathfindingList[Index].Cost,
					DelayedSpiltPathfindingList[Index].Parent
				});
				OutCanMoveToArray[DelayedSpiltPathfindingList[Index].Index].Index = 0;
				OutCanMoveToArray[DelayedSpiltPathfindingList[Index].Index].Cost = DelayedSpiltPathfindingList[Index].
					Cost;
				OutCanMoveToArray[DelayedSpiltPathfindingList[Index].Index].Parent = DelayedSpiltPathfindingList[Index].
					Parent;
				DelayedSpiltPathfindingList.RemoveAt(Index);
			}
			else
			{
				++Index;
			}
		}
	}
	else
	{
		DelayedSpiltPathfindingList.Empty();
		ResetPathfindingArraysAndClearMeshes();
		if (PathfindingMove <= 0)
		{
			OutCanMoveToArray[CurrentIndex] = {CurrentIndex, 0, CurrentIndex};
			OutIndexCanMoveToArray.Add({CurrentIndex, 0, CurrentIndex});
			return;
		}
		OpenList.Add({CurrentIndex, 0, CurrentIndex});
	}
	// todo.. pathfinding type, current is standard
	for (int32 SearchStep = CurrentSearchStep; SearchStep < PathfindingMove; ++SearchStep)
	{
		SearchAndAddAdjacentTiles(DelayedSpiltPathfindingList, OpenList, OpenListChildren, DelaySearchList,
		                          bShowStartIndex, InStartIndex, bExcludeFriendly);
		// 完成上一步寻路
		OpenList.Empty();
		OutIndexCanMoveToArray.Append(OpenListChildren);
		// 移动到下一步寻路
		CurrentSearchStep += 1;
		// 将待寻路Tile加入到OpenList中
		OpenList.Append(OpenListChildren);
		OpenList.Append(DelaySearchList);
		OpenListChildren.Empty();
		DelaySearchList.Empty();
		// 判断是否终止搜索
		if (OpenList.IsEmpty())
		{
			break;
		}
		if (CurrentSearchStep >= PathfindingMove)
		{
			break;
		}
	}
	// 结束寻路
	OutCanMoveToArray[CurrentIndex] = {CurrentIndex, 0, CurrentIndex};
	OutIndexCanMoveToArray.Add({CurrentIndex, 0, CurrentIndex});
	// todo... for path finding friendly
	for (const auto& CanMove : OutIndexCanMoveToArray)
	{
		if (bShowStartIndex && CanMove.Index == InStartIndex) continue;
		OutTileIndexes.Add(CanMove.Index);
	}
}

void AGridMapManager::DisplayTileIndexesInternal()
{
	for (const auto& TextRender : DisplayIndexesTextRenderArray)
	{
		TextRender->DestroyComponent();
	}
	DisplayIndexesTextRenderArray.Empty();
	for (int32 Index = 0; Index < VectorFieldArray.Num(); ++Index)
	{
		if (UKismetMathLibrary::NotEqual_VectorVector(VectorFieldArray[Index], FVector::Zero(), 1.0f))
		{
			FTransform NewTransform;
			NewTransform.SetLocation({
				VectorFieldArray[Index].X, VectorFieldArray[Index].Y, VectorFieldArray[Index].Z + 10.0f
			});
			NewTransform.SetRotation(FQuat({90.0f, 0.0f, 270.0f}));
			NewTransform.SetScale3D(FVector::One());
			UTextRenderComponent* TextRender = nullptr;
			TextRender = Cast<UTextRenderComponent>(
				this->AddComponentByClass(UTextRenderComponent::StaticClass(), false, NewTransform, false));
			DisplayIndexesTextRenderArray.Add(TextRender);
			TextRender->SetText(FText::AsNumber(Index));
		}
	}
	if (!VectorFieldArray.IsEmpty())
	{
		if (UKismetMathLibrary::EqualEqual_VectorVector(VectorFieldArray[0], FVector::Zero(), 0.0001))
		{
			FTransform NewTransform;
			NewTransform.SetLocation({VectorFieldArray[0].X, VectorFieldArray[0].Y, VectorFieldArray[0].Z + 10.0f});
			NewTransform.SetRotation(FQuat({90.0f, 0.0f, 270.0f}));
			NewTransform.SetScale3D(FVector::One());
			UTextRenderComponent* TextRender = Cast<UTextRenderComponent>(
				this->AddComponentByClass(UTextRenderComponent::StaticClass(), false, NewTransform, false));
			DisplayIndexesTextRenderArray.Add(TextRender);
			TextRender->SetText(FText::AsNumber(0));
		}
	}
}

void AGridMapManager::DisplayTileEdgesInternal()
{
	if (DisplayEdgeInstancedMeshComponent == nullptr)
	{
		const FTransform NewTransform;
		DisplayEdgeInstancedMeshComponent = Cast<UInstancedStaticMeshComponent>(
			this->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, NewTransform, false));
		DisplayEdgeInstancedMeshComponent->SetStaticMesh(EdgeArrowStaticMesh);
	}
	for (const auto& EdgeCost : DisplayEdgeCostTextRenderArray)
	{
		EdgeCost->DestroyComponent();
	}
	DisplayEdgeInstancedMeshComponent->ClearInstances();
	DisplayEdgeCostTextRenderArray.Empty();
	for (int32 Index = 0; Index < EdgeArray.Num(); ++Index)
	{
		FVector TileLocation = VectorFieldArray[Index];
		if (UKismetMathLibrary::NotEqual_VectorVector(TileLocation, FVector::Zero(), 1.0f))
		{
			for (const auto& Edge : EdgeArray[Index].Array)
			{
				const FVector EdgeLocation = VectorFieldArray[Edge.Index];
				FTransform NewTransform;
				NewTransform.SetLocation({
					(EdgeLocation.X + TileLocation.X + TileLocation.X) / 3.0f,
					(EdgeLocation.Y + TileLocation.Y + TileLocation.Y) / 3.0f, TileLocation.Z + 10.0f
				});
				NewTransform.SetScale3D(FVector::One());
				if (Edge.Cost > 1)
				{
					NewTransform.SetRotation(FQuat({
						90.0f, UKismetMathLibrary::FindLookAtRotation(TileLocation, EdgeLocation).Yaw, 0.0f
					}));
					UTextRenderComponent* TextRender = Cast<UTextRenderComponent>(
						this->AddComponentByClass(UTextRenderComponent::StaticClass(), false, NewTransform, false));
					TextRender->SetText(FText::AsNumber(Edge.Cost));
					DisplayEdgeCostTextRenderArray.Add(TextRender);
				}
				else
				{
					NewTransform.SetRotation(FQuat({
						0.0f, UKismetMathLibrary::FindLookAtRotation(EdgeLocation, TileLocation).Yaw, 0.0f
					}));
					DisplayEdgeInstancedMeshComponent->AddInstance(NewTransform, false);
				}
			}
		}
	}

	if (!EdgeArray.IsEmpty())
	{
		const FVector TileLocation = VectorFieldArray[0];
		for (const auto& Edge : EdgeArray[0].Array)
		{
			const FVector EdgeLocation = VectorFieldArray[Edge.Index];
			FTransform NewTransform;
			NewTransform.SetLocation({
				(EdgeLocation.X + TileLocation.X + TileLocation.X) / 3.0f,
				(EdgeLocation.Y + TileLocation.Y + TileLocation.Y) / 3.0f, TileLocation.Z + 10.0f
			});
			NewTransform.SetScale3D(FVector::One());
			if (Edge.Cost > 1)
			{
				NewTransform.SetRotation(FQuat({
					90.0f, UKismetMathLibrary::FindLookAtRotation(TileLocation, EdgeLocation).Yaw, 0.0f
				}));
				UTextRenderComponent* TextRender = Cast<UTextRenderComponent>(
					this->AddComponentByClass(UTextRenderComponent::StaticClass(), false, NewTransform, false));
				TextRender->SetText(FText::AsNumber(Edge.Cost));
				DisplayEdgeCostTextRenderArray.Add(TextRender);
			}
			else
			{
				NewTransform.SetRotation(FQuat({
					0.0f, UKismetMathLibrary::FindLookAtRotation(EdgeLocation, TileLocation).Yaw, 0.0f
				}));
				DisplayEdgeInstancedMeshComponent->AddInstance(NewTransform, false);
			}
		}
	}
}

void AGridMapManager::InitializeVisibleComponent() const
{
	DefaultTileInstance->ClearInstances();
	DefaultTileInstance->SetStaticMesh(DefaultStaticMesh);
	CollisionPlane->SetWorldLocation(GetActorLocation() + CollisionLocation);
	CollisionPlane->SetWorldScale3D(CollisionScale);
	CollisionPlane->SetVisibility(bShowCollision);
	Heightmap->SetWorldLocation(GetActorLocation() + HeightmapLocation);
	Heightmap->SetWorldScale3D(HeightmapScale);
	Heightmap->SetVisibility(bShowHeightmap);
	if (bShowDefaultTile)
	{
		for (const auto& Tile : VectorFieldArray)
		{
			FTransform TileTransform;
			TileTransform.SetLocation(FVector(Tile.X, Tile.Y, 0.0f));
			DefaultTileInstance->AddInstance(TileTransform);
		}
	}
}

void AGridMapManager::InitializeDecalAndPathComponent()
{
	// 首先需要计算贴花的大小
	DecalSizeSquare.X = (TileBoundsX + TileBoundsY) * 0.5f;
	DecalSizeSquare.Y = TileBoundsX * 0.4375f;
	DecalSizeSquare.Z = TileBoundsY * 0.4375f;

	// 设置贴花材质，并计算其大小
	HoverMarkerDecal = UGridMapFunctionLibrary::CreateGridMapDecalComponent(HoverMarkerDecalMaterial, DecalSizeSquare,
	                                                                        GetWorld(), this, 0.0f);
	HoverMarkerDecal->SetWorldLocationAndRotation(FVector(0, 0, 0), FRotator(90.0f, 90.0f, 0.0f));
	ScaleDecalRelativeToDefaultTile(HoverMarkerDecal);
	HoverMarkerDecal->SetVisibility(false);
	CurrentMarkerDecal = UGridMapFunctionLibrary::CreateGridMapDecalComponent(
		CurrentMarkerDecalMaterial, DecalSizeSquare,
		GetWorld(), this, 0.0f);
	CurrentMarkerDecal->SetWorldLocationAndRotation(FVector(0, 0, 0), FRotator(90.0f, 90.0f, 0.0f));
	ScaleDecalRelativeToDefaultTile(CurrentMarkerDecal);
	CurrentMarkerDecal->SetVisibility(false);
	TargetMarkerDecal = UGridMapFunctionLibrary::CreateGridMapDecalComponent(TargetMarkerDecalMaterial, DecalSizeSquare,
	                                                                         GetWorld(), this, 0.0f);
	TargetMarkerDecal->SetWorldLocationAndRotation(FVector(0, 0, 0), FRotator(90.0f, 90.0f, 0.0f));
	ScaleDecalRelativeToDefaultTile(TargetMarkerDecal);
	TargetMarkerDecal->SetVisibility(false);

	// 设置箭头材质
	const FTransform SplineTransform;
	PathSpline = Cast<USplineComponent>(
		AddComponentByClass(USplineComponent::StaticClass(), false, SplineTransform, false));
	DisplaySpline = Cast<USplineComponent>(
		AddComponentByClass(USplineComponent::StaticClass(), false, SplineTransform, false));
	SplineMeshLength = (TileBoundsX + TileBoundsY) / 2.0f;

	// 路径材质，只针对于是Mesh的网格
	const FTransform PathMeshTransform;
	Path = Cast<UInstancedStaticMeshComponent>(
		AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, PathMeshTransform, false));
	Path->SetStaticMesh(PathMesh);
}

void AGridMapManager::ScaleDecalRelativeToDefaultTile(UDecalComponent* DecalComponent) const
{
	// 计算贴花相对于地形Tile的大小
	const float MaxValue = FMath::Max(TileBoundsX, TileBoundsY);
	const FVector OldScale = DecalComponent->GetComponentScale();
	const float NewScaleX = OldScale.X * (MaxValue / 460.0f);
	const float NewScaleY = OldScale.Y * (TileBoundsX / 230.0f);
	const float NewScaleZ = OldScale.Z * (TileBoundsX / 230.0f);
	DecalComponent->SetWorldScale3D(FVector(NewScaleX, NewScaleY, NewScaleZ));
}

void AGridMapManager::CreateSplinePath(const TArray<int32>& InPathIndexArray)
{
	PathVectorArray.Empty();
	for (const int32 PathIndex : InPathIndexArray)
	{
		PathVectorArray.Add(IndexToVectorOnGrid(PathIndex, SplineHeight));
	}
	PathSpline->SetSplinePoints(PathVectorArray, ESplineCoordinateSpace::World, true);
}

void AGridMapManager::DisplayPathAsSpline()
{
	DestroyAndClearSplinePath();

	DisplaySpline->SetSplinePoints(PathVectorArray, ESplineCoordinateSpace::World, true);
	for (int32 Index = 0; Index < PathVectorArray.Num(); ++Index)
	{
		DisplaySpline->SetSplinePointType(Index, ESplinePointType::Linear, true);
	}
	double Remainder = 0.0f;
	const int32 EndPoint = UKismetMathLibrary::FMod64(DisplaySpline->GetSplineLength(), SplineMeshLength, Remainder);
	for (int32 Index = 0; Index < EndPoint; ++Index)
	{
		FTransform SplineTransform;
		auto SplineMesh = Cast<USplineMeshComponent>(
			this->AddComponentByClass(USplineMeshComponent::StaticClass(), true, SplineTransform, false));
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetStaticMesh(SplinePathMesh);
		SplineMesh->SetMaterial(0, PathMaterial);
		SplineMesh->SetStartAndEnd(
			DisplaySpline->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World),
			DisplaySpline->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World) *
			SplineMeshLength,
			DisplaySpline->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength,
			                                                ESplineCoordinateSpace::World),
			DisplaySpline->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength,
			                                                 ESplineCoordinateSpace::World) * SplineMeshLength,
			true
		);
		SplineMeshArray.Add(SplineMesh);
	}
}

void AGridMapManager::ResetPathfindingArraysAndClearMeshes()
{
	ClearAllInRangeMeshMarkers();
	IndexCanMoveToArray.Empty();
	ReachablePawnsArray.Empty();
	CanMoveToArray.Empty();
	for (int32 Index = 0; Index < GridSizeX * GridSizeY * GridSizeZ; ++Index)
	{
		CanMoveToArray.Add({0, 0, 0});
	}
	CurrentSearchStep = 0;
}

void AGridMapManager::ClearAllInRangeMeshMarkers()
{
	for (const auto& Decal : CurrentDecalsArray)
	{
		Decal->DestroyComponent();
	}
	CurrentDecalsArray.Empty();
}

void AGridMapManager::SetMarkerVisibility(bool bVisibility, UDecalComponent* DecalComponent)
{
	DecalComponent->SetVisibility(bVisibility);
}

void AGridMapManager::SetMarkerLocationUseIndex(int32 Index, UDecalComponent* DecalComponent)
{
	check(VectorFieldArray.IsValidIndex(Index));
	DecalComponent->SetRelativeLocation(VectorFieldArray[Index], false);
}

void AGridMapManager::SetMarkerLocationUseLocation(const FVector& Location, UDecalComponent* DecalComponent)
{
	DecalComponent->SetRelativeLocation(Location - GetActorLocation(), false);
}

void AGridMapManager::RemoveDisplayedPath()
{
	DestroyAndClearSplinePath();
}

void AGridMapManager::DestroyAndClearSplinePath()
{
	Path->ClearInstances();
	for (const auto& SplineMesh : SplineMeshArray)
	{
		SplineMesh->DestroyComponent();
	}
	SplineMeshArray.Empty();
}
