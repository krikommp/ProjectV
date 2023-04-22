// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapFunctionLibrary.h"

#include "GridLogChannel.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GridMapManager/GridMapManager.h"
#include "GridMapManager/GridMapStruct.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GridTraceChannel.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"


void UGridMapFunctionLibrary::CreateNewGridMapDataAsset(AGridMapManager* GridMapManager)
{
	ConstructGridMapData(GridMapManager);
}

UDecalComponent* UGridMapFunctionLibrary::CreateGridMapDecalComponent(UMaterialInterface* DecalMaterial,
                                                                      FVector DecalSize, UWorld* World, AActor* Actor,
                                                                      float LifeSpan)
{
	if (World && World->GetNetMode() == NM_DedicatedServer)
	{
		return nullptr;
	}

	UDecalComponent* DecalComp = NewObject<UDecalComponent>((Actor ? Actor : (UObject*)World));
	DecalComp->bAllowAnyoneToDestroyMe = true;
	DecalComp->SetDecalMaterial(DecalMaterial);
	DecalComp->DecalSize = DecalSize;
	DecalComp->SetUsingAbsoluteScale(true);
	DecalComp->RegisterComponentWithWorld(World);

	if (LifeSpan > 0.f)
	{
		DecalComp->SetLifeSpan(LifeSpan);
	}

	return DecalComp;
}

void UGridMapFunctionLibrary::VectorToIndex3DNative(AGridMapManager* GridMapManager, const FVector& Location,
                                                    int32& Index, float& Z)
{
	const float PivotX = (GridMapManager->TileBoundsX * 0.5) + Location.X;
	const float PivotY = (GridMapManager->TileBoundsY * 0.5) + Location.Y;

	const float ModX = FMath::Floor(FMath::Fmod(PivotX, GridMapManager->TileBoundsX));
	const float ModY = FMath::Floor(FMath::Fmod(PivotY, GridMapManager->TileBoundsY));

	const int AddX = ModX ? 1 : 0;
	const int AddY = ModY ? 1 : 0;

	const int X = FMath::Floor(((PivotX + AddX) - GridMapManager->GetActorLocation().X) / GridMapManager->TileBoundsX);
	const int Y = FMath::Floor(((PivotY + AddY) - GridMapManager->GetActorLocation().Y) / GridMapManager->TileBoundsY) *
		GridMapManager->
		GridSizeX;

	Index = X + Y;

	if (Index < 0) Index = 0;
	const FVector TilePos = GridMapManager->VectorFieldArray[Index];
	const FVector ActorLocation = GridMapManager->GetActorLocation();

	TArray<float> LocalFloatArray;
	TArray<float> FloatCompareArray;
	int LocalLevels = 0;
	float LocalStartZ = 0.0f;

	while (true)
	{
		const float StartZ = (TilePos.Z + GridMapManager->MaxGridHeight) + ActorLocation.Z;
		const float EndZ = (TilePos.Z + GridMapManager->MinGridHeight) + ActorLocation.Z;

		const TArray<AActor*> IgnoreActor;
		FHitResult Hit;
		UKismetSystemLibrary::LineTraceSingle(
			GridMapManager->GetWorld(),
			FVector(TilePos.X + ActorLocation.X, TilePos.Y + ActorLocation.Y, StartZ),
			FVector(TilePos.X + ActorLocation.X, TilePos.Y + ActorLocation.Y, EndZ),
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			Hit,
			false);

		const FVector FirstHitLocation = Hit.Location;
		float RelativeZ = FirstHitLocation.Z - ActorLocation.Z;
		LocalFloatArray.Add(RelativeZ);
		float CompareZ = FMath::Abs(RelativeZ - (Location.Z - ActorLocation.Z));
		FloatCompareArray.Add(CompareZ);

		LocalStartZ = FirstHitLocation.Z - GridMapManager->HeightBetweenLevels;
		float LocalMinGridHeight = ActorLocation.Z + GridMapManager->MinGridHeight;
		if (LocalStartZ <= LocalMinGridHeight)
		{
			break;
		}
		UKismetSystemLibrary::LineTraceSingle(
			GridMapManager->GetWorld(),
			FVector(FirstHitLocation.X, FirstHitLocation.Y, LocalStartZ),
			FVector(FirstHitLocation.X, FirstHitLocation.Y, LocalMinGridHeight),
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			Hit,
			false);
		if (!Hit.IsValidBlockingHit())
		{
			break;
		}
		FVector SecondHitLocation = Hit.Location;
		UKismetSystemLibrary::LineTraceSingle(
			GridMapManager->GetWorld(),
			FVector(SecondHitLocation.X, SecondHitLocation.Y, SecondHitLocation.Z + 0.5f),
			FVector(SecondHitLocation.X, SecondHitLocation.Y,
			        SecondHitLocation.Z + (GridMapManager->HeightBetweenLevels - 0.5f)),
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			Hit,
			false);
		if (!Hit.IsValidBlockingHit())
		{
			// 高度足够
			LocalFloatArray.Add(SecondHitLocation.Z - ActorLocation.Z);
			FloatCompareArray.Add(FMath::Abs((SecondHitLocation.Z - ActorLocation.Z) - (Location.Z - ActorLocation.Z)));
		}
		if ((SecondHitLocation.Z - GridMapManager->HeightBetweenLevels > LocalMinGridHeight) && ((LocalLevels + 2) <
			GridMapManager->MaxLevels))
		{
			// 可以进行下一次循环
			LocalStartZ = SecondHitLocation.Z - GridMapManager->HeightBetweenLevels;
			++LocalLevels;
		}
		else
		{
			break;
		}
	}

	// 找到最小的Z值
	int32 IndexOfMinValue;
	FMath::Min(FloatCompareArray, &IndexOfMinValue);
	Index = Index + (FloatCompareArray.Num() - 1 - IndexOfMinValue) * GridMapManager->GridSizeX * GridMapManager->
		GridSizeY;

	Z = ActorLocation.Z + LocalFloatArray[IndexOfMinValue];
}

int32 UGridMapFunctionLibrary::VectorToIndex3D(AGridMapManager* GridMapManager, const FVector& Vector)
{
	const float PivotX = (GridMapManager->TileBoundsX * 0.5) + Vector.X;
	const float PivotY = (GridMapManager->TileBoundsY * 0.5) + Vector.Y;

	const float ModX = FMath::Floor(FMath::Fmod(PivotX, GridMapManager->TileBoundsX));
	const float ModY = FMath::Floor(FMath::Fmod(PivotY, GridMapManager->TileBoundsY));

	const int AddX = ModX ? 1 : 0;
	const int AddY = ModY ? 1 : 0;

	const int X = FMath::Floor(((PivotX + AddX) - GridMapManager->GetActorLocation().X) / GridMapManager->TileBoundsX);
	const int Y = FMath::Floor(((PivotY + AddY) - GridMapManager->GetActorLocation().Y) / GridMapManager->TileBoundsY) *
		GridMapManager->
		GridSizeX;

	const int32 Result = X + Y;

	for (int32 Index = 0; Index <= GridMapManager->GridSizeZ - 1; ++Index)
	{
		if (GridMapManager->VectorFieldArray.IsValidIndex(
			Index * (GridMapManager->GridSizeX * GridMapManager->GridSizeY) + Result))
		{
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(
				GridMapManager->VectorFieldArray[Index * (GridMapManager->GridSizeX * GridMapManager->GridSizeY) +
					Result].Z,
				Vector.Z - GridMapManager->GetActorLocation().Z, GridMapManager->HeightBetweenLevels / 2.0f))
			{
				return (Index * (GridMapManager->GridSizeX * GridMapManager->GridSizeY) + Result);
			}
		}
	}

	return Result;
}

void UGridMapFunctionLibrary::DisplayGridMapIndexAndEdge(AGridMapManager* GridMapManager)
{
#if WITH_EDITOR
	if (GridMapManager->bDisplayTileIndex)
	{
		GridMapManager->DisplayTileIndexesInternal();
	}

	if (GridMapManager->bDisplayTileEdge)
	{
		GridMapManager->DisplayTileEdgesInternal();
	}
#endif
}

static float CompareTwoVector(float A, float B)
{
	return FMath::Sqrt(A * A + B * B);
}

bool UGridMapFunctionLibrary::CompareClickLocation(AGridMapManager* GridMapManager, const FVector& ClickedLocation,
                                                   int32 ClickedIndex)
{
	if (UKismetMathLibrary::EqualEqual_VectorVector(
		ClickedLocation,
		GridMapManager->VectorFieldArray[ClickedIndex] + GridMapManager->GetActorLocation(),
		CompareTwoVector(GridMapManager->TileBoundsX, GridMapManager->TileBoundsY)))
	{
		return true;
	}
	return false;
}

void UGridMapFunctionLibrary::DisplayMoveRangeEdgeMarkers(AGridMapManager* GridMapManager,
                                                          const TArray<FStructPathFinding>& InCanMoveToArray,
                                                          const TArray<FStructPathFinding>& InIndexCanMoveToArray)
{
	for (const auto& CanMoveTo : InIndexCanMoveToArray)
	{
		SpawnEdgeMeshes(GridMapManager, InCanMoveToArray, GridMapManager->TileInMoveRangeEdgeDecal, CanMoveTo.Index);
	}
}

void UGridMapFunctionLibrary::DisplayInsightRangeEdgeMarkers(AGridMapManager* GridMapManager,
                                                             const TArray<int32>& InTilesInSightArray,
                                                             const TArray<int32>& InRangeArray)
{
	for (const int32 Index : InTilesInSightArray)
	{
		SpawnEdgeMeshes(GridMapManager, InRangeArray, GridMapManager->TileInSightRangeEdgeDecal,
		                Index);
	}
}

void UGridMapFunctionLibrary::RemoveTileEdge(int32 TileIndex, int32 Edge, AGridMapManager* GridMapManager)
{
	// 在TileIndex的邻居中查找Edge的索引
	// 如果存在，那么就将TileIndex索引对应的队列中的Edge移除掉
	FStructIntArray LocalIntArray = GridMapManager->EdgeArrayInteger[TileIndex];
	FStructEdgeArray LocalArrayTile = GridMapManager->EdgeArray[TileIndex];
	// 根据传入的Edge在LocalIntArray查找Index, -1为没有查找到
	const int32 EdgeIndex = LocalIntArray.Index.Find(Edge);
	if (EdgeIndex != -1)
	{
		LocalArrayTile.Array.RemoveAt(EdgeIndex);
		LocalIntArray.Index.RemoveAt(EdgeIndex);
		GridMapManager->EdgeArrayInteger[TileIndex] = LocalIntArray;
		GridMapManager->EdgeArray[TileIndex] = LocalArrayTile;
	}
}

void UGridMapFunctionLibrary::RemoveTileEdgeBothWays(int32 Index1, int32 Index2, AGridMapManager* GridMapManager)
{
	RemoveTileEdge(Index1, Index2, GridMapManager);
	RemoveTileEdge(Index2, Index1, GridMapManager);
}

void UGridMapFunctionLibrary::SetEdgeCost(int32 TileIndex, int32 Edge, int32 NewCost, AGridMapManager* GridMapManager)
{
	const FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[TileIndex];
	const int32 EdgeIndex = IntArray.Index.Find(Edge);
	if (EdgeIndex != -1)
	{
		FStructEdgeArray LocalStructArray = GridMapManager->EdgeArray[TileIndex];
		LocalStructArray.Array[EdgeIndex].Index = Edge;
		LocalStructArray.Array[EdgeIndex].Cost = NewCost;

		GridMapManager->EdgeArray[TileIndex] = LocalStructArray;
	}
}

void UGridMapFunctionLibrary::RemoveEdgesOnEastAndWestSideOfGrid(int TileIndex, AGridMapManager* GridMapManager)
{
	// 根据传进来的Index, 在EdgeArrayInteger中查找对应的索引
	const FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[TileIndex];
	const int Length = IntArray.Index.Num();
	for (int i = Length - 1; i >= 0; --i)
	{
		const int Edge = IntArray.Index[i];
		// 只有当VectorFieldArray的索引有效才是一个可以被移除的Tile
		if (Edge > -1 && Edge < GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ)
		{
			const FVector Tile = GridMapManager->VectorFieldArray[TileIndex];
			const FVector EdgeTile = GridMapManager->VectorFieldArray[Edge];
			// 根据两个Tile的X轴坐标来判断，如果X轴坐标相差过大，超过了一个TileBoundsX，就说明这两个Tile不是邻居
			const int Distance = FMath::Abs(Tile.X - EdgeTile.X);
			if (Distance > GridMapManager->TileBoundsX)
			{
				// 执行移除操作
				RemoveTileEdge(TileIndex, Edge, GridMapManager);
			}
		}
	}
}

void UGridMapFunctionLibrary::RemoveEdgesOnNorthAndSouthSideOfGrid(int TileIndex, AGridMapManager* GridMapManager)
{
	const FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[TileIndex];
	const int Length = IntArray.Index.Num();
	for (int i = Length - 1; i >= 0; --i)
	{
		const int Edge = IntArray.Index[i];
		// 只有当VectorFieldArray的索引有效才是一个可以被移除的Tile
		if (Edge > -1 && Edge < GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ)
		{
			const FVector Tile = GridMapManager->VectorFieldArray[TileIndex];
			const FVector EdgeTile = GridMapManager->VectorFieldArray[Edge];
			// 根据两个Tile的Y轴坐标来判断，如果X轴坐标相差过大，超过了一个TileBoundsY，就说明这两个Tile不是邻居
			const int Distance = FMath::Abs(Tile.Y - EdgeTile.Y);
			if (Distance > GridMapManager->TileBoundsY)
			{
				// 执行移除操作
				RemoveTileEdge(TileIndex, Edge, GridMapManager);
			}
		}
	}
}

void UGridMapFunctionLibrary::BlockCornersInBetweenDiagonalTiles(int32 Index1, int32 Index2,
                                                                 AGridMapManager* GridMapManager)
{
	const int32 RowIndex1 = Index1 / GridMapManager->GridSizeX;
	const int32 RowIndex2 = Index2 / GridMapManager->GridSizeX;

	const int32 ColIndex1 = Index1 % GridMapManager->GridSizeX;
	const int32 ColIndex2 = Index2 % GridMapManager->GridSizeX;
	if (RowIndex1 > RowIndex2)
	{
		if (ColIndex1 - ColIndex2 > 0)
		{
			// 东北方向
			RemoveTileEdgeBothWays(Index1 - 1, Index1 - GridMapManager->GridSizeX, GridMapManager);
		}
		else if (ColIndex1 - ColIndex2 < 0)
		{
			// 西北方向
			RemoveTileEdgeBothWays(Index1 + 1, Index1 - GridMapManager->GridSizeX, GridMapManager);
		}
		else if (ColIndex1 - ColIndex2 == 0)
		{
			return;
		}
	}
	else if (RowIndex1 < RowIndex2)
	{
		if (ColIndex1 - ColIndex2 > 0)
		{
			// 东南方向
			RemoveTileEdgeBothWays(Index1 + GridMapManager->GridSizeX, Index1 - 1, GridMapManager);
		}
		else if (ColIndex1 - ColIndex2 < 0)
		{
			// 西南方向
			RemoveTileEdgeBothWays(Index1 + GridMapManager->GridSizeX, Index1 + 1, GridMapManager);
		}
		else if (ColIndex1 - ColIndex2 == 0)
		{
			return;
		}
	}
	else if (RowIndex1 == RowIndex2)
	{
		return;
	}
}

void UGridMapFunctionLibrary::SetCollisionPlaneAndHeightmapBoxScaleAndLocation(AGridMapManager* GridMapManager,
                                                                               FVector& Location, float& ScaleX,
                                                                               float& ScaleY)
{
	ScaleX = (GridMapManager->GridSizeX) * (GridMapManager->TileBoundsX / 200.0f);
	ScaleY = (GridMapManager->GridSizeY) * (GridMapManager->TileBoundsY / 200.0f);
	// 注意我们需要计算的是正中间
	const float X = ((GridMapManager->GridSizeX * GridMapManager->TileBoundsX) / 2.0f) - (GridMapManager->TileBoundsX /
		2.0f);
	const float Y = ((GridMapManager->GridSizeY * GridMapManager->TileBoundsY) / 2.0f) - (GridMapManager->TileBoundsY /
		2.0f);

	Location = FVector(X, Y, 0.1);
}

FVector UGridMapFunctionLibrary::IndexToVectorSquareGrid(int32 Index, AGridMapManager* GridMapManager)
{
	// 首先算X轴的值
	const int32 ResidueX = Index % GridMapManager->GridSizeX; // 这是求Index整除GridSizeX后剩余的值，这个剩余的值就是X轴上的偏移
	const float X = GridMapManager->TileBoundsX * ResidueX;

	// 接着算Y轴的值
	const int32 ResidueYZ = Index / GridMapManager->GridSizeX;
	// 如果Z轴为1，那么上一步其实也就求出了Y轴的偏移，但是如果考虑到Z轴不为1，则需要移除Z轴的影响
	const int32 SquaredXY = GridMapManager->GridSizeX * GridMapManager->GridSizeY; // 先求出整个XY平面
	const int32 ResidueZ = Index / SquaredXY; // 接着求出Z轴的偏移
	const int32 ResidueXZ = GridMapManager->GridSizeX * ResidueZ;
	const int32 ResidueY = ResidueYZ - ResidueXZ; // 注意这个例子中当GridSizeX,GridSizeY是不一样的时候，算出的Y值会有偏移
	const float Y = ResidueY * GridMapManager->TileBoundsY;

	// 接着算Z轴的值
	const float Z = ResidueZ * 200.0f;

	return FVector(X, Y, Z);
}

void UGridMapFunctionLibrary::PopulateEdgeTileAndCost(AGridMapManager* GridMapManager)
{
	const int32 Length = GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ;
	for (int32 Index = 0; Index < Length; ++Index)
	{
		if (GridMapManager->IsDiagonalMovement())
		{
			const int32 North = Index - GridMapManager->GridSizeX;
			const int32 East = Index + 1;
			const int32 South = Index + GridMapManager->GridSizeX;
			const int32 West = Index - 1;
			const int32 NorthEast = Index - GridMapManager->GridSizeX + 1;
			const int32 SouthEast = Index + GridMapManager->GridSizeX + 1;
			const int32 SouthWest = Index + GridMapManager->GridSizeX - 1;
			const int32 NorthWest = Index - GridMapManager->GridSizeX - 1;
			const TArray TempIntArray = {North, East, South, West, NorthEast, SouthEast, SouthWest, NorthWest};
			const TArray<FStructIndexCost> TempIndexCostArray = {
				{North, 1},
				{East, 1},
				{South, 1},
				{West, 1},
				{NorthEast, 1},
				{SouthEast, 1},
				{SouthWest, 1},
				{NorthWest, 1}
			};
			GridMapManager->EdgeArrayInteger.Add({TempIntArray});
			GridMapManager->EdgeArray.Add({TempIndexCostArray});
		}
		else
		{
			const int32 North = Index - GridMapManager->GridSizeX;
			const int32 East = Index + 1;
			const int32 South = Index + GridMapManager->GridSizeX;
			const int32 West = Index - 1;
			const TArray TempIntArray = {North, East, South, West};
			const TArray<FStructIndexCost> TempIndexCostArray = {{North, 1}, {East, 1}, {South, 1}, {West, 1}};
			GridMapManager->EdgeArrayInteger.Add({TempIntArray});
			GridMapManager->EdgeArray.Add({TempIndexCostArray});
		}
	}
}

void UGridMapFunctionLibrary::AddViewportTerrainToArrays(AGridMapManager* GridMapManager)
{
	AutoGeneratorHeightAndRotation(GridMapManager);
	PrepareForCreateMultiLevelGrids(GridMapManager);
	CreateWallsOnGridEdges(GridMapManager);
	AutoCalcEdgeBaseOnHeight(GridMapManager);
	CollectAllTileParentOnGridMapAndAddToEdgeArray(GridMapManager);
}

void UGridMapFunctionLibrary::AutoGeneratorHeightAndRotation(AGridMapManager* GridMapManager)
{
	const int32 Length = GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ;
	for (int32 Index = 0; Index < Length; ++Index)
	{
		const FVector TilePos = GridMapManager->VectorFieldArray[Index];
		const FVector ActorLocation = GridMapManager->GetActorLocation();
		const float StartX = ActorLocation.X + TilePos.X;
		const float StartY = ActorLocation.Y + TilePos.Y;
		const float StartZ = ActorLocation.Z + (TilePos.Z + GridMapManager->MaxGridHeight);

		const float EndX = StartX;
		const float EndY = StartY;
		const float EndZ = ActorLocation.Z + (TilePos.Z + GridMapManager->MinGridHeight);

		TArray<AActor*> IgnoreActor;
		FHitResult Hit;
		UKismetSystemLibrary::LineTraceSingle(
			GridMapManager->GetWorld(),
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
			GridMapManager->VectorFieldArray[Index].Z = HitLocation.Z - GridMapManager->GetActorLocation().Z;
			AddMultiLevelGridAtIndex(GridMapManager, Index, HitLocation);
		}
	}
}

void UGridMapFunctionLibrary::CreateWallsOnGridEdges(AGridMapManager* GridMapManager)
{
	// 这个函数的目的是为了移除网格边缘的Tile,因为在之前根据方向构建的Array没有考虑到边缘的情况
	// 例如，对于(0, 0) 这个Tile, 它是不存在west这个方向上的tile的
	// 所以我们需要在这里把这些方向上的Tile给处理掉
	const int32 Length = GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ;
	for (int32 Index = 0; Index < Length; ++Index)
	{
		// 首先找到所有的west方向不应该存在的Tile
		if ((Index % GridMapManager->GridSizeX) == 0)
		{
			// 执行移除操作
			RemoveEdgesOnEastAndWestSideOfGrid(Index, GridMapManager);
		}
		else if (((Index + 1) % GridMapManager->GridSizeX) == 0)
		{
			// 接着找到所有east方向不应该存在的Tile
			RemoveEdgesOnEastAndWestSideOfGrid(Index, GridMapManager);
		}
		else if ((Index % (GridMapManager->GridSizeX * GridMapManager->GridSizeY)) < GridMapManager->GridSizeX)
		{
			const int32 Squared = GridMapManager->GridSizeX * GridMapManager->GridSizeY;
			const int32 Remain = Index / Squared;
			const int32 ZIndex = Remain * Squared;
			FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[Index];
			for (int32 i = IntArray.Index.Num() - 1; i >= 0; --i)
			{
				const int32 Edge = IntArray[i];
				if (Edge < (ZIndex) && Edge > (ZIndex - GridMapManager->GridSizeX))
				{
					RemoveTileEdge(Index, Edge, GridMapManager);
				}
			}
			RemoveEdgesOnNorthAndSouthSideOfGrid(Index, GridMapManager);
		}
		else if ((Index % (GridMapManager->GridSizeX * GridMapManager->GridSizeY)) >= ((GridMapManager->GridSizeX *
			GridMapManager->GridSizeY) - GridMapManager->GridSizeX))
		{
			const int32 Squared = GridMapManager->GridSizeX * GridMapManager->GridSizeY;
			const int32 Remain = Index / Squared;
			const int32 Bottom = (Remain + 1) * Squared - 1;
			const int32 Top = Bottom + GridMapManager->GridSizeX;
			FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[Index];
			for (int32 i = IntArray.Index.Num() - 1; i >= 0; --i)
			{
				const int32 Edge = IntArray[i];
				if (Edge > Bottom && Edge < Top)
				{
					RemoveTileEdge(Index, Edge, GridMapManager);
				}
			}

			RemoveEdgesOnNorthAndSouthSideOfGrid(Index, GridMapManager);
		}
	}

	for (int Index = 0; Index < GridMapManager->EdgeArrayInteger.Num(); ++Index)
	{
		FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[Index];
		for (int i = 0; i < IntArray.Index.Num(); ++i)
		{
			if (IntArray[i] < 0)
			{
				RemoveTileEdge(Index, IntArray[i], GridMapManager);
			}
			if (IntArray[i] >= GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ)
			{
				RemoveTileEdge(Index, IntArray[i], GridMapManager);
			}
		}
	}
}

void UGridMapFunctionLibrary::AutoCalcEdgeBaseOnHeight(AGridMapManager* GridMapManager)
{
	const int32 Length = GridMapManager->VectorFieldArray.Num();
	for (int32 Index = 0; Index < Length; ++Index)
	{
		const FVector TilePos = GridMapManager->VectorFieldArray[Index];
		FStructEdgeArray EdgeCost = GridMapManager->EdgeArray[Index];
		for (int32 EdgeIndex = EdgeCost.Array.Num() - 1; EdgeIndex >= 0; --EdgeIndex)
		{
			const FStructIndexCost IndexCost = EdgeCost[EdgeIndex];
			const float ParentZ = TilePos.Z;
			const FVector EdgeTilePos = GridMapManager->VectorFieldArray[IndexCost.Index];
			const float Height = FMath::Abs(ParentZ - EdgeTilePos.Z);
			int32 NewCost = -1;
			if (Height < GridMapManager->HeightSlowIncrement)
			{
				// 不需要花费额外的行动力了就可以到达的Edge,这里就不需要再做其他的处理了
				NewCost = 1;
			}
			else
			{
				if (Height < GridMapManager->HeightImpassableCutoff)
				{
					// 需要花费额外的行动力才能到达的Edge
					NewCost = FMath::Floor(Height / GridMapManager->HeightSlowIncrement) + 1;
				}
				else
				{
					// 需要被切断，即不可以到达的Edge
					NewCost = 0;
				}
			}

			switch (NewCost)
			{
			case 0:
				// 需要从寻路网格中剔除
				BlockCornersInBetweenDiagonalTiles(IndexCost.Index, Index, GridMapManager);
				RemoveTileEdgeBothWays(IndexCost.Index, Index, GridMapManager);
				break;
			case 1:
				// 不需要进行额外操作
				break;
			default:
				// 其他情况，根据NewCost，设定其行动力消耗
				SetEdgeCost(IndexCost.Index, Index, NewCost, GridMapManager);
				SetEdgeCost(Index, IndexCost.Index, NewCost, GridMapManager);
				break;;
			}
		}
	}
}

void UGridMapFunctionLibrary::TraceForWallsFunction(AGridMapManager* GridMapManager)
{
	TArray<int32> LocalIndexArray;
	for (int32 TileIndex = 0; TileIndex < GridMapManager->EdgeArrayInteger.Num(); ++TileIndex)
	{
		// 对于每一个Tile
		LocalIndexArray.Empty();
		// 向他的邻居发送一条射线，检测是否有Wall
		FStructIntArray IntArray = GridMapManager->EdgeArrayInteger[TileIndex];
		for (int EdgeIndex = 0; EdgeIndex < IntArray.Index.Num(); ++EdgeIndex)
		{
			FVector TilePos = GridMapManager->VectorFieldArray[TileIndex] + GridMapManager->GetActorLocation();
			FVector EdgePos = GridMapManager->VectorFieldArray[IntArray.Index[EdgeIndex]] + GridMapManager->
				GetActorLocation();
			TilePos.Z += GridMapManager->TraceForWallsHeight;
			EdgePos.Z += GridMapManager->TraceForWallsHeight;

			TArray<AActor*> IgnoreActor;
			FHitResult Hit;
			UKismetSystemLibrary::LineTraceSingle(
				GridMapManager->GetWorld(),
				EdgePos,
				TilePos,
				UEngineTypes::ConvertToTraceType(WallTrace),
				false,
				IgnoreActor,
				EDrawDebugTrace::None,
				Hit,
				true);
			if (Hit.IsValidBlockingHit())
			{
				// 如果存在Wall，那么就把这个wall下面的Tile索引加入到LocalIndexArray
				LocalIndexArray.Add(IntArray.Index[EdgeIndex]);
			}
		}
		// 如果LocalIndexArray不为空，需要把这个Tile从寻路网格中删除
		for (int Index = 0; Index < LocalIndexArray.Num(); ++Index)
		{
			RemoveTileEdgeBothWays(TileIndex, LocalIndexArray[Index], GridMapManager);
		}
	}
}

void UGridMapFunctionLibrary::ConstructGridMapData(AGridMapManager* GridMapManager)
{
	GridMapManager->DefaultTileInstance->ClearInstances();
	GridMapManager->VectorFieldArray.Empty();
	GridMapManager->CanMoveToArray.Empty();
	GridMapManager->EdgeArray.Empty();
	GridMapManager->EdgeArrayInteger.Empty();
	GridMapManager->GridSizeZ = 1;
	// step 1 初始化
	int32 GridSquared = GridMapManager->GridSizeX * GridMapManager->GridSizeY;
	GridMapManager->DefaultTileInstance->SetStaticMesh(GridMapManager->DefaultStaticMesh);
	FVector MinBounds, MaxBounds;
	GridMapManager->DefaultTileInstance->GetLocalBounds(MinBounds, MaxBounds);
	GridMapManager->TileBoundsX = MaxBounds.X - MinBounds.X;
	GridMapManager->TileBoundsY = MaxBounds.Y - MinBounds.Y;
	// step 2 设置CollisionPlane大小
	FTransform CollisionTransform;
	float CollisionScaleX, CollisionScaleY;
	FVector CollisionLocation;
	SetCollisionPlaneAndHeightmapBoxScaleAndLocation(GridMapManager, CollisionLocation, CollisionScaleX,
	                                                 CollisionScaleY);
	GridMapManager->CollisionLocation = CollisionLocation;
	GridMapManager->CollisionScale = FVector(CollisionScaleX, CollisionScaleY, 1.0);
	CollisionTransform.SetLocation(GridMapManager->CollisionLocation + GridMapManager->GetActorLocation());
	CollisionTransform.SetScale3D(GridMapManager->CollisionScale);
	GridMapManager->CollisionPlane->SetWorldTransform(CollisionTransform);
	GridMapManager->CollisionPlane->SetVisibility(GridMapManager->bShowCollision);
	// step 3 设置HeightMap大小
	FVector HeightmapLocation;
	float HeightmapScaleX, HeightmapScaleY;
	SetCollisionPlaneAndHeightmapBoxScaleAndLocation(GridMapManager, HeightmapLocation, HeightmapScaleX,
	                                                 HeightmapScaleY);
	const float HalfHeight = (GridMapManager->MaxGridHeight + GridMapManager->MinGridHeight) / 2.0f;
	const float ScaleHeight = (GridMapManager->MaxGridHeight - GridMapManager->MinGridHeight) / 200.0f;
	HeightmapLocation.Z = HalfHeight;
	GridMapManager->HeightmapLocation = HeightmapLocation;
	GridMapManager->HeightmapScale = FVector(HeightmapScaleX, HeightmapScaleY, ScaleHeight);
	FTransform HeightmapTransform;
	HeightmapTransform.SetLocation(GridMapManager->HeightmapLocation + GridMapManager->GetActorLocation());
	HeightmapTransform.SetScale3D(GridMapManager->HeightmapScale);
	GridMapManager->Heightmap->SetWorldTransform(HeightmapTransform);
	GridMapManager->Heightmap->SetVisibility(GridMapManager->bShowHeightmap);
	GridMapManager->Heightmap->SetBoxExtent(FVector(100, 100, 100));

	// setup 3. 创建矢量网格，这是世界移动以及寻路的基础
	const int32 Count = GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		GridMapManager->VectorFieldArray.Add(IndexToVectorSquareGrid(Index, GridMapManager));
	}

	// setup 4. 创建可视化网格
	if (GridMapManager->bShowDefaultTile)
	{
		for (const auto& TilePos : GridMapManager->VectorFieldArray)
		{
			FTransform TileTransform;
			TileTransform.SetLocation(TilePos);
			GridMapManager->DefaultTileInstance->AddInstance(TileTransform);
		}
	}

	// 预生成
	PreGeneratorGridMap(GridMapManager);
}

void UGridMapFunctionLibrary::PreGeneratorGridMap(AGridMapManager* GridMapManager)
{
	// alloc grid array space
	const int32 Length = GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ;
	GridMapManager->CanMoveToArray.SetNum(Length);

	// 生成每个Tile的相邻信息（不考虑边缘及特殊情况）
	PopulateEdgeTileAndCost(GridMapManager);
	if (!GridMapManager->bCollisionPlaneWalkable)
	{
		GridMapManager->CollisionPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	AddViewportTerrainToArrays(GridMapManager);
	TraceForWallsFunction(GridMapManager);
}

void UGridMapFunctionLibrary::AddMultiLevelGridAtIndex(AGridMapManager* GridMapManager, int32 InIndex,
                                                       const FVector& StartLocation)
{
	const FVector GridLocationInMap = GridMapManager->VectorFieldArray[InIndex];
	TArray<FVector> LocalVectorArray;
	const FVector ActorLocation = GridMapManager->GetActorLocation();
	// 计算相对高度
	LocalVectorArray.Add(FVector(GridLocationInMap.X, GridLocationInMap.Y, StartLocation.Z - ActorLocation.Z));
	// 计算射线开始位置
	float LocalStartLocationZ = StartLocation.Z - GridMapManager->HeightBetweenLevels;
	// 计算实际地图最小高度
	const float LocalMinGridHeight = GridMapManager->MinGridHeight + ActorLocation.Z;
	// 当且仅当实际最小高度低于射线开始高度时才开始计算
	int32 LocalLevels = 0;
	if (LocalStartLocationZ > LocalMinGridHeight)
	{
		while (true)
		{
			// 我们从射线开始位置向下至地图最低位置发送一条射线
			TArray<AActor*> IgnoreActors;
			FHitResult Hit;
			UKismetSystemLibrary::LineTraceSingle(
				GridMapManager->GetWorld(),
				FVector(StartLocation.X, StartLocation.Y, LocalStartLocationZ),
				FVector(StartLocation.X, StartLocation.Y, LocalMinGridHeight),
				UEngineTypes::ConvertToTraceType(PathTrace),
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				Hit,
				false);
			// 如果发生了Hit
			if (Hit.IsValidBlockingHit())
			{
				// 我们再从Hit位置向上发射一条射线
				// 用于检测这个位置的高度是否满足人物通行
				const FVector HitLocation = Hit.Location;
				IgnoreActors.Empty();
				UKismetSystemLibrary::LineTraceSingle(
					GridMapManager->GetWorld(),
					FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + 0.5),
					FVector(HitLocation.X, HitLocation.Y, HitLocation.Z + 0.5 + GridMapManager->HeightBetweenLevels),
					UEngineTypes::ConvertToTraceType(PathTrace),
					false,
					IgnoreActors,
					EDrawDebugTrace::None,
					Hit,
					false);
				if (!Hit.IsValidBlockingHit())
				{
					// 如果没有发生Hit, 说明这个高度是可以通行的
					LocalVectorArray.Add(FVector(GridLocationInMap.X, GridLocationInMap.Y,
					                             HitLocation.Z - ActorLocation.Z));
				}
				// 再次判断再向下一层是否满足高度
				// 并且新增层数小于设定的最大层数
				if (((HitLocation.Z - GridMapManager->HeightBetweenLevels) > LocalMinGridHeight) && (LocalLevels + 2) <
					GridMapManager->MaxLevels)
				{
					++LocalLevels;
					LocalStartLocationZ = HitLocation.Z - GridMapManager->HeightBetweenLevels;
				}
				else
				{
					break;
				}
			}
			else
			{
				// 如果没有发生Hit, 说明这个位置不满足人物行走条件
				break;
			}
		}
		// 现在我们完成了射线检测部分，所有需要被加入到寻路系统的节点已经被寻找到
		// 我们重新计算他们的索引，并将他们加入到 VectorArray 中
		for (int32 Index = 0; Index < LocalVectorArray.Num(); ++Index)
		{
			const int32 LastIndex = LocalVectorArray.Num() - 1;
			const int32 NewIndex = (LastIndex - Index) * (GridMapManager->GridSizeX * GridMapManager->GridSizeY) +
				InIndex;
			if (!GridMapManager->VectorFieldArray.IsValidIndex(NewIndex))
			{
				GridMapManager->VectorFieldArray.SetNumZeroed(NewIndex + 1);
			}
			GridMapManager->VectorFieldArray[NewIndex] = LocalVectorArray[Index];
		}
	}
}

void UGridMapFunctionLibrary::PrepareForCreateMultiLevelGrids(AGridMapManager* GridMapManager)
{
	// 首先判断是否是多层级的地图
	const int32 GridMapSquareSize = GridMapManager->GridSizeX * GridMapManager->GridSizeY;
	if (GridMapManager->VectorFieldArray.Num() <= GridMapSquareSize)
	{
		// 说明地图不是多层级的，直接跳过生成
		return;
	}
	// 重新计算出Z轴大小
	GridMapManager->GridSizeZ += ((GridMapManager->VectorFieldArray.Num() - 1) / GridMapSquareSize);
	const int32 AllSize = GridMapManager->GridSizeX * GridMapManager->GridSizeY * GridMapManager->GridSizeZ;
	// 补全缺少的Tile
	for (int32 Index = GridMapManager->VectorFieldArray.Num(); Index < AllSize; ++Index)
	{
		GridMapManager->VectorFieldArray.Add(FVector(0, 0, 0));
	}
	// 扩张地图数组大小
	GridMapManager->CanMoveToArray.SetNum(AllSize);
	GridMapManager->EdgeArrayInteger.SetNum(AllSize);
	GridMapManager->EdgeArray.SetNum(AllSize);

	// 遍历第一层以上的所有Tile
	for (int32 Index = GridMapSquareSize; Index < (GridMapSquareSize * GridMapManager->GridSizeZ) - 1; ++Index)
	{
		// 判断是否是空闲的 Tile
		if (UKismetMathLibrary::NotEqual_VectorVector(GridMapManager->VectorFieldArray[Index], FVector(0, 0, 0),
		                                              0.0001))
		{
			// 尝试计算出该Tile中的所有相邻Tile索引
			TArray<int32> AdjacentIndexArray = GetAdjacentIndexes(GridMapManager, Index, false);
			// 由于两个相邻的 Tile 可能不在同一层，所以我们找出所有层上与这个Tile相邻的索引
			for (const auto& AdjacentIndex : AdjacentIndexArray)
			{
				for (int32 ZIndex = GridMapManager->GridSizeZ - 1; ZIndex >= 0; --ZIndex)
				{
					const int32 ChildIndex = (ZIndex * GridMapSquareSize) + (AdjacentIndex % GridMapSquareSize);
					if (UKismetMathLibrary::NotEqual_VectorVector(GridMapManager->VectorFieldArray[ChildIndex],
					                                              FVector(0, 0, 0), 0.0001))
					{
						// 根据高度判断行动力消耗
						// 对于结果为 0 的，认为是不可以通过的地方
						const int32 Cost = GetEdgeCostFromZDifferent(GridMapManager,
						                                             GridMapManager->VectorFieldArray[Index].Z,
						                                             ChildIndex);
						if (Cost > 0)
						{
							// 找到符合条件的Tile, 结束查找
							AddEdgeBothWays(GridMapManager, Index, ChildIndex, Cost);
							break;
						}
					}
				}
			}
		}
	}
	if (GridMapManager->IsDiagonalMovement())
	{
		// 在添加完直线后添加对角线边缘，以便在寻路过程中首选直角边移动
		// 遍历第一层以上的所有Tile
		for (int32 Index = GridMapSquareSize; Index < (GridMapSquareSize * GridMapManager->GridSizeZ) - 1; ++Index)
		{
			// 判断是否是空闲的 Tile
			if (UKismetMathLibrary::NotEqual_VectorVector(GridMapManager->VectorFieldArray[Index], FVector(0, 0, 0),
			                                              0.0001))
			{
				// 尝试计算出该Tile中的所有相邻Tile索引
				TArray<int32> AdjacentIndexArray = GetAdjacentIndexes(GridMapManager, Index, true);
				// 由于两个相邻的 Tile 可能不在同一层，所以我们找出所有层上与这个Tile相邻的索引
				for (const auto& AdjacentIndex : AdjacentIndexArray)
				{
					for (int32 ZIndex = GridMapManager->GridSizeZ - 1; ZIndex >= 0; --ZIndex)
					{
						const int32 ChildIndex = (ZIndex * GridMapSquareSize) + (AdjacentIndex % GridMapSquareSize);
						if (UKismetMathLibrary::NotEqual_VectorVector(GridMapManager->VectorFieldArray[ChildIndex],
						                                              FVector(0, 0, 0), 0.0001))
						{
							// 根据高度判断行动力消耗
							// 对于结果为 0 的，认为是不可以通过的地方
							const int32 Cost = GetEdgeCostFromZDifferent(GridMapManager,
							                                             GridMapManager->VectorFieldArray[Index].Z,
							                                             ChildIndex);
							if (Cost > 0)
							{
								// 找到符合条件的Tile, 结束查找
								AddEdgeBothWays(GridMapManager, Index, ChildIndex, Cost);
								break;
							}
						}
					}
				}
			}
		}
	}
}

TArray<int32> UGridMapFunctionLibrary::GetAdjacentIndexes(AGridMapManager* GridMapManager, int32 Index, bool bDiagonal)
{
	TArray<int32> AdjacentIndexes;
	if (bDiagonal)
	{
		AdjacentIndexes = {
			Index - GridMapManager->GridSizeX, Index + 1, Index + GridMapManager->GridSizeX, Index - 1
		};
	}
	else
	{
		AdjacentIndexes = {
			Index - GridMapManager->GridSizeX + 1, Index + GridMapManager->GridSizeX + 1,
			Index + GridMapManager->GridSizeX - 1, Index - GridMapManager->GridSizeX - 1
		};
	}
	TArray<int32> LocalIndexArray;
	const int32 GridMapSquareSize = GridMapManager->GridSizeX * GridMapManager->GridSizeY;
	for (const auto& AdjacentIndex : AdjacentIndexes)
	{
		for (int32 ZIndex = 0; ZIndex < GridMapManager->GridSizeZ; ++ZIndex)
		{
			LocalIndexArray.Add((AdjacentIndex % GridMapSquareSize) + (GridMapSquareSize * ZIndex));
		}
	}
	return LocalIndexArray;
}

int32 UGridMapFunctionLibrary::GetEdgeCostFromZDifferent(AGridMapManager* GridMapManager, float ParentZ,
                                                         int32 ChildIndex)
{
	const FVector ChildLocation = GridMapManager->VectorFieldArray[ChildIndex];
	if (FMath::Abs(ChildLocation.Z - ParentZ) < GridMapManager->HeightSlowIncrement)
	{
		return 1;
	}
	if (FMath::Abs(ChildLocation.Z - ParentZ) < GridMapManager->HeightImpassableCutoff)
	{
		return FMath::Floor(FMath::Abs(ChildLocation.Z - ParentZ) / GridMapManager->HeightSlowIncrement) + 1;
	}
	return 0;
}

void UGridMapFunctionLibrary::AddEdgeBothWays(AGridMapManager* GridMapManager, int32 TileIndex, int32 Edge, int32 Cost)
{
	AddEdge(GridMapManager, TileIndex, Edge, Cost);
	AddEdge(GridMapManager, Edge, TileIndex, Cost);
}

void UGridMapFunctionLibrary::AddEdge(AGridMapManager* GridMapManager, int32 TileIndex, int32 Edge, int32 Cost)
{
	auto LocalStructArray = GridMapManager->EdgeArray[TileIndex];
	auto LocalIntArray = GridMapManager->EdgeArrayInteger[TileIndex];
	const int32 FindIndex = GridMapManager->EdgeArrayInteger[TileIndex].Index.Find(Edge);
	if (FindIndex != INDEX_NONE)
	{
		LocalStructArray[FindIndex] = {Edge, Cost};
	}
	else
	{
		LocalStructArray.Array.Add({Edge, Cost});
		LocalIntArray.Index.Add(Edge);
		GridMapManager->EdgeArrayInteger[TileIndex] = LocalIntArray;
	}
	GridMapManager->EdgeArray[TileIndex] = LocalStructArray;
}

void UGridMapFunctionLibrary::CollectAllTileParentOnGridMapAndAddToEdgeArray(AGridMapManager* GridMapManager)
{
	TArray<AActor*> Outer;
	UGameplayStatics::GetAllActorsOfClass(GridMapManager->GetWorld(), AGridTileParent::StaticClass(), Outer);
	for (const auto& Actor : Outer)
	{
		if (AGridTileParent* TileParent = Cast<AGridTileParent>(Actor))
		{
			if (!GridMapManager->VectorFieldArray.IsValidIndex(TileParent->TileIndex))
			{
				UE_LOG(LogGrid, Error, TEXT("TileParent [%s] with [%d] Tile Index Is InValid, and i will skip it!"),
				       *GetNameSafe(TileParent), TileParent->TileIndex);
				break;
			}
			AddTileEdgesToEdgeArray(GridMapManager, TileParent);
			for (const auto& CustomEdge : TileParent->CustomEdges)
			{
				if (CustomEdge.CostTo != INDEX_NONE)
				{
					AddEdge(GridMapManager, TileParent->TileIndex, CustomEdge.Index, CustomEdge.CostTo);
				}
				if (CustomEdge.CostBack != INDEX_NONE)
				{
					AddEdge(GridMapManager, CustomEdge.Index, TileParent->TileIndex, CustomEdge.CostBack);
				}
			}
			for (const auto& BlockEdgeIndex : TileParent->BlockedEdges)
			{
				RemoveTileEdgeBothWays(TileParent->TileIndex, BlockEdgeIndex, GridMapManager);
			}
		}
	}
}

void UGridMapFunctionLibrary::AddTileEdgesToEdgeArray(AGridMapManager* GridMapManager, AGridTileParent* Tile)
{
	FStructEdgeArray TileEdgeArray = GridMapManager->EdgeArray[Tile->TileIndex];
	TArray<int32> LocalIntegerArray;
	TArray<FStructIndexCost> LocalEdgeStructArray;
	for (const auto& TileEdge : TileEdgeArray.Array)
	{
		const int32 EdgeIndex = Tile->EdgeIndexes.Find(TileEdge.Index);
		if (EdgeIndex != INDEX_NONE)
		{
			LocalIntegerArray.Add(TileEdge.Index);
			LocalEdgeStructArray.Add({TileEdge.Index, TileEdge.Cost});
			SetEdgeCost(TileEdge.Index, Tile->TileIndex,
			            FMath::Max(Tile->EdgeCostStructArray[EdgeIndex].Cost, TileEdge.Cost), GridMapManager);
		}
		else
		{
			// 在自定义Tile中，该edge不存在，那么就从地图寻路网格中移除这个edge
			RemoveTileEdge(TileEdge.Index, Tile->TileIndex, GridMapManager);
		}
	}
	GridMapManager->EdgeArrayInteger[Tile->TileIndex] = {LocalIntegerArray};
	GridMapManager->EdgeArray[Tile->TileIndex] = {LocalEdgeStructArray};
	// todo...
}

void UGridMapFunctionLibrary::SpawnEdgeMeshes(AGridMapManager* GridMapManager,
                                              const TArray<FStructPathFinding>& InCanMoveToArray,
                                              UMaterialInterface* DecalMat, int32 Index)
{
	const FVector StraightEdgeDecalSize = {90.0, 90.0, 90.0};
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InCanMoveToArray, Index, Index + 1, StraightEdgeDecalSize, 270.0,
	                             DecalMat);
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InCanMoveToArray, Index, Index - 1, StraightEdgeDecalSize, 90.0,
	                             DecalMat);
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InCanMoveToArray, Index, Index + GridMapManager->GridSizeX,
	                             StraightEdgeDecalSize, 0.0, DecalMat);
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InCanMoveToArray, Index, Index - GridMapManager->GridSizeX,
	                             StraightEdgeDecalSize, 180.0, DecalMat);
}

void UGridMapFunctionLibrary::SpawnEdgeMeshes(AGridMapManager* GridMapManager,
                                              const TArray<int32>& InRangeArray, UMaterialInterface* DecalMat,
                                              int32 Index)
{
	const FVector StraightEdgeDecalSize = {90.0, 90.0, 90.0};
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InRangeArray, Index, Index + 1,
	                             StraightEdgeDecalSize, 270.0,
	                             DecalMat);
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InRangeArray, Index, Index - 1,
	                             StraightEdgeDecalSize, 90.0,
	                             DecalMat);
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InRangeArray, Index,
	                             Index + GridMapManager->GridSizeX,
	                             StraightEdgeDecalSize, 0.0, DecalMat);
	SpawnEdgeDecalBetweenIndexes(GridMapManager, InRangeArray, Index,
	                             Index - GridMapManager->GridSizeX,
	                             StraightEdgeDecalSize, 180.0, DecalMat);
}

void UGridMapFunctionLibrary::SpawnEdgeDecalBetweenIndexes(AGridMapManager* GridMapManager,
                                                           const TArray<FStructPathFinding>& InCanMoveToArray,
                                                           int32 OutsideIndex, int32 InsideIndex,
                                                           const FVector& DecalSize,
                                                           float Rotation, UMaterialInterface* DecalMat)
{
	if (!InCanMoveToArray.IsValidIndex(InsideIndex))
	{
		return;
	}
	// 不是边缘Tile, 因此不需要绘制
	if (InCanMoveToArray[InsideIndex].Parent != 0)
	{
		return;
	}
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
		GridMapManager->GetWorld(), DecalMat, DecalSize,
		GridMapManager->VectorFieldArray[OutsideIndex] + GridMapManager->GetActorLocation(), {90.0f, Rotation, 90.0});
	GridMapManager->CurrentDecalsArray.Add(DecalComponent);
}

void UGridMapFunctionLibrary::SpawnEdgeDecalBetweenIndexes(AGridMapManager* GridMapManager,
                                                           const TArray<int32>& InRangeArray, int32 OutsideIndex,
                                                           int32 InsideIndex, const FVector& DecalSize, float Rotation,
                                                           UMaterialInterface* DecalMat)
{
	if (!InRangeArray.IsValidIndex(InsideIndex))
	{
		return;
	}
	if (InRangeArray[InsideIndex] != 0)
	{
		return;
	}
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
		GridMapManager->GetWorld(), DecalMat, DecalSize,
		GridMapManager->VectorFieldArray[OutsideIndex] + GridMapManager->GetActorLocation(), {90.0f, Rotation, 90.0});
	GridMapManager->CurrentDecalsArray.Add(DecalComponent);
}
