#include "Tilemap3DPathfindingGenerator.h"
#include "ProjectVEditor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tilemap/TilemapAsset.h"
#include "Tilemap/TileSet3DAsset.h"

void FTilemap3DPathfindingGenerator::Setup(const UObject* WorldContextObject, UTilemapAsset* InTilemapAsset,
                                           const UTileSet3DAsset* InTileSetAsset)
{
	InTilemapAsset->LevelSizeZ = 1;
	const int32 Count = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY * InTilemapAsset->LevelSizeZ;
	InTilemapAsset->PathFindingBlocks.SetNum(Count);
	for (int32 Index = 0; Index < Count; ++Index)
	{
		InTilemapAsset->PathFindingBlocks[Index].Location = IndexToVector(InTilemapAsset, Index);
		PopulateEdgeTileAndCost(InTilemapAsset, Index, InTileSetAsset->bDiagonalMovement);
		ComputeTilemapSurfaceHeight(WorldContextObject, InTilemapAsset, Index);
	}
	CreateMultiLevelGrids(InTilemapAsset, InTileSetAsset->bDiagonalMovement);
}

FVector FTilemap3DPathfindingGenerator::IndexToVector(const UTilemapAsset* InTilemapAsset, int32 Index)
{
	const int32 ResidueX = Index % InTilemapAsset->LevelSizeX;
	const float X = InTilemapAsset->GridSize * ResidueX;

	const int32 ResidueYZ = Index / InTilemapAsset->LevelSizeX;
	const int32 SquaredXY = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY;
	const int32 ResidueZ = Index / SquaredXY;
	const int32 ResidueXZ = InTilemapAsset->LevelSizeX * ResidueZ;
	const int32 ResidueY = ResidueYZ - ResidueXZ;
	const float Y = ResidueY * InTilemapAsset->GridSize;

	const float Z = ResidueZ * InTilemapAsset->GridSize;

	return FVector(X, Y, Z);
}

void FTilemap3DPathfindingGenerator::PopulateEdgeTileAndCost(UTilemapAsset* InTilemapAsset, int32 Index,
                                                             bool bDiagonalMovement)
{
	TArray<int32> TempIntArray;
	TArray<FTilemapMovementStep> TempMoveStep;
	if (bDiagonalMovement)
	{
		const int32 North = Index - InTilemapAsset->LevelSizeX;
		const int32 East = Index + 1;
		const int32 South = Index + InTilemapAsset->LevelSizeX;
		const int32 West = Index - 1;
		const int32 NorthEast = Index - InTilemapAsset->LevelSizeX + 1;
		const int32 SouthEast = Index + InTilemapAsset->LevelSizeX + 1;
		const int32 SouthWest = Index + InTilemapAsset->LevelSizeX - 1;
		const int32 NorthWest = Index - InTilemapAsset->LevelSizeX - 1;
		TempIntArray = {North, East, South, West, NorthEast, SouthEast, SouthWest, NorthWest};
		TempMoveStep = {
			{North, 1},
			{East, 1},
			{South, 1},
			{West, 1},
			{NorthEast, 1},
			{SouthEast, 1},
			{SouthWest, 1},
			{NorthWest, 1}
		};
	}
	else
	{
		const int32 North = Index - InTilemapAsset->LevelSizeX;
		const int32 East = Index + 1;
		const int32 South = Index + InTilemapAsset->LevelSizeX;
		const int32 West = Index - 1;
		TempIntArray = {North, East, South, West};
		TempMoveStep = {{North, 1}, {East, 1}, {South, 1}, {West, 1}};
	}
	InTilemapAsset->PathFindingBlocks[Index].EdgeArrayIndex = TempIntArray;
	InTilemapAsset->PathFindingBlocks[Index].EdgeArray = TempMoveStep;
}

void FTilemap3DPathfindingGenerator::ComputeTilemapSurfaceHeight(const UObject* WorldContextObject,
                                                                 UTilemapAsset* InTilemapAsset, int32 Index)
{
	const FVector TilePos = InTilemapAsset->PathFindingBlocks[Index].Location;
	const float StartX = TilePos.X;
	const float StartY = TilePos.Y;
	const float StartZ = TilePos.Z + InTilemapAsset->GetMaxLevelHeight();

	const float EndX = StartX;
	const float EndY = StartY;
	const float EndZ = TilePos.Z + InTilemapAsset->GetMinLevelHeight();

	TArray<AActor*> IgnoreActor;
	FHitResult Hit;
	UKismetSystemLibrary::LineTraceSingle(
		WorldContextObject,
		FVector(StartX, StartY, StartZ),
		FVector(EndX, EndY, EndZ),
		UEngineTypes::ConvertToTraceType(PathTrace),
		false,
		IgnoreActor,
		EDrawDebugTrace::None,
		Hit,
		false);
	if (Hit.IsValidBlockingHit())
	{
		const FVector HitLocation = Hit.Location;
		InTilemapAsset->PathFindingBlocks[Index].Location.Z = HitLocation.Z;
		AddMultiLevelGridAtIndex(WorldContextObject, InTilemapAsset, Index, HitLocation);
	}
}

void FTilemap3DPathfindingGenerator::AddMultiLevelGridAtIndex(const UObject* WorldContextObject,
                                                              UTilemapAsset* InTilemapAsset, int32 InIndex,
                                                              const FVector& StartLocation)
{
	const FVector GridLocationInMap = InTilemapAsset->PathFindingBlocks[InIndex].Location;
	TArray<FVector> LocalVectorArray;
	LocalVectorArray.Add(FVector(GridLocationInMap.X, GridLocationInMap.Y, StartLocation.Z));
	float LocalStartLocationZ = StartLocation.Z - InTilemapAsset->HeightBetweenLevel;
	const float LocalMinGridHeight = InTilemapAsset->GetMinLevelHeight();

	if (LocalStartLocationZ > LocalMinGridHeight)
	{
		int32 LocalLevels = 0;
		while (true)
		{
			TArray<AActor*> IgnoreActors;
			FHitResult Hit;
			UKismetSystemLibrary::LineTraceSingle(
				WorldContextObject,
				FVector(StartLocation.X, StartLocation.Y, LocalStartLocationZ),
				FVector(StartLocation.X, StartLocation.Y, LocalMinGridHeight),
				UEngineTypes::ConvertToTraceType(PathTrace),
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				Hit,
				false);
			if (Hit.IsValidBlockingHit())
			{
				const FVector HitLocation = Hit.Location;
				IgnoreActors.Empty();
				UKismetSystemLibrary::LineTraceSingle(
					WorldContextObject,
					FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + 0.5),
					FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + 0.5 + InTilemapAsset->HeightBetweenLevel),
					UEngineTypes::ConvertToTraceType(PathTrace),
					false,
					IgnoreActors,
					EDrawDebugTrace::None,
					Hit,
					false);
				if (!Hit.IsValidBlockingHit())
				{
					LocalVectorArray.Add(FVector(GridLocationInMap.X, GridLocationInMap.Y,
					                             HitLocation.Z));
				}
				if (((HitLocation.Z - InTilemapAsset->HeightBetweenLevel) > LocalMinGridHeight) && (LocalLevels + 2) <
					InTilemapAsset->Floors)
				{
					LocalLevels += 2;
					LocalStartLocationZ = HitLocation.Z - InTilemapAsset->HeightBetweenLevel;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		for (int32 Index = 0; Index < LocalVectorArray.Num(); ++Index)
		{
			const int32 LastIndex = LocalVectorArray.Num() - 1;
			const int32 NewIndex = (LastIndex - Index) * (InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY) +
				Index;
			if (!InTilemapAsset->PathFindingBlocks.IsValidIndex(NewIndex))
			{
				InTilemapAsset->PathFindingBlocks.SetNumZeroed(NewIndex + 1);
			}
			InTilemapAsset->PathFindingBlocks[NewIndex].Location = LocalVectorArray[Index];
		}
	}
}

void FTilemap3DPathfindingGenerator::CreateMultiLevelGrids(UTilemapAsset* InTilemapAsset, bool bDiagonalMovement)
{
	// 首先判断是否是多层级的地图
	const int32 GridMapSquareSize = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeX;
	if (InTilemapAsset->PathFindingBlocks.Num() <= GridMapSquareSize)
	{
		// 说明地图不是多层级的，直接跳过生成
		return;
	}
	// 重新计算出Z轴大小
	InTilemapAsset->LevelSizeZ += ((InTilemapAsset->PathFindingBlocks.Num() - 1) / GridMapSquareSize);
	const int32 AllSize = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeZ;
	// 补全缺少的Tile
	for (int32 Index = InTilemapAsset->PathFindingBlocks.Num(); Index < AllSize; ++Index)
	{
		FTilemapPathFindingBlock NewPathFindingBlock;
		InTilemapAsset->PathFindingBlocks.Add(NewPathFindingBlock);
	}
	for (int32 Index = GridMapSquareSize; Index < (GridMapSquareSize * InTilemapAsset->LevelSizeZ) - 1; ++Index)
	{
		// 判断是否是空闲的 Tile
		if (UKismetMathLibrary::NotEqual_VectorVector(InTilemapAsset->PathFindingBlocks[Index].Location,
		                                              FVector(0, 0, 0),
		                                              0.0001))
		{
			// 尝试计算出该Tile中的所有相邻Tile索引
			TArray<int32> AdjacentIndexArray = GetAdjacentIndexes(InTilemapAsset, Index, bDiagonalMovement);
			// 由于两个相邻的 Tile 可能不在同一层，所以我们找出所有层上与这个Tile相邻的索引
			for (const auto& AdjacentIndex : AdjacentIndexArray)
			{
				for (int32 ZIndex = InTilemapAsset->LevelSizeZ - 1; ZIndex >= 0; --ZIndex)
				{
					const int32 ChildIndex = (ZIndex * GridMapSquareSize) + (AdjacentIndex % GridMapSquareSize);
					if (UKismetMathLibrary::NotEqual_VectorVector(
						InTilemapAsset->PathFindingBlocks[ChildIndex].Location,
						FVector(0, 0, 0), 0.0001))
					{
						// 根据高度判断行动力消耗
						// 对于结果为 0 的，认为是不可以通过的地方
						const int32 Cost = InTilemapAsset->GetEdgeCostFromZDifferent(Index, ChildIndex);
						if (Cost > 0)
						{
							// 找到符合条件的Tile, 结束查找
							InTilemapAsset->AddEdgeBothWays(Index, ChildIndex, Cost);
							break;
						}
					}
				}
			}
		}
	}
}

TArray<int32> FTilemap3DPathfindingGenerator::GetAdjacentIndexes(const UTilemapAsset* InTilemapAsset, int32 Index,
                                                                 bool bDiagonalMovement)
{
	TArray<int32> AdjacentIndexes = {
		Index - InTilemapAsset->LevelSizeX, Index + 1, Index + InTilemapAsset->LevelSizeY, Index - 1
	};
	if (bDiagonalMovement)
	{
		AdjacentIndexes.Append({
			Index - InTilemapAsset->LevelSizeX + 1, Index + InTilemapAsset->LevelSizeX + 1,
			Index + InTilemapAsset->LevelSizeX - 1, Index - InTilemapAsset->LevelSizeX - 1
		});
	}
	TArray<int32> LocalIndexArray;
	const int32 GridMapSquareSize = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY;
	for (const auto& AdjacentIndex : AdjacentIndexes)
	{
		for (int32 ZIndex = 0; ZIndex < InTilemapAsset->LevelSizeZ; ++ZIndex)
		{
			LocalIndexArray.Add((AdjacentIndex % GridMapSquareSize) + (GridMapSquareSize * ZIndex));
		}
	}
	return LocalIndexArray;
}

void FTilemap3DPathfindingGenerator::CreateWallsOnGridEdges(UTilemapAsset* InTilemapAsset)
{
	const int32 Count = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY * InTilemapAsset->LevelSizeZ;
}
