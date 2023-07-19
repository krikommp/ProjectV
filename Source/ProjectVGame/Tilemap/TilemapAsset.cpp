// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapAsset.h"

void FTilemap3DTerrainMeshData::Clear()
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	Colors.Empty();
	UV0.Empty();
	VertexCount = 0;
}

int32 UTilemapAsset::GetBlockIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Z * LevelSizeX * LevelSizeY + Y * LevelSizeX + X;
}

int32 UTilemapAsset::VectorToIndex(const FVector& Location, int32 Floor) const
{
	const float PivotX = (GridSize * 0.5) + Location.X;
	const float PivotY = (GridSize * 0.5) + Location.Y;

	const float ModX = FMath::Floor(FMath::Fmod(PivotX, GridSize));
	const float ModY = FMath::Floor(FMath::Fmod(PivotY, GridSize));

	const int AddX = ModX ? 1 : 0;
	const int AddY = ModY ? 1 : 0;

	const int X = FMath::Floor(((PivotX + AddX)) / GridSize);
	const int Y = FMath::Floor(((PivotY + AddY)) / GridSize) *
		LevelSizeX;

	const int32 Result = X + Y;

	return Result + LevelSizeX * LevelSizeY * Floor;
}

int32 UTilemapAsset::PathFindingBlockToBlock(int32 Index) const
{
	ensureAlwaysMsgf(PathFindingBlocks.IsValidIndex(Index),
	                 TEXT("UTilemapAsset::PathFindingBlockToBlock, [%d] is invalid index in PathFinding blocks"),
	                 Index);

	const FTilemapPathFindingBlock PathFindingBlock = PathFindingBlocks[Index];
	const FVector TileLocation = PathFindingBlock.Location;

	const int32 Floor = FMath::Floor(TileLocation.Z / GridSize);

	return VectorToIndex(TileLocation, Floor);
}

int32 UTilemapAsset::GetEdgeCostFromZDifferent(int32 SelfIndex, int32 OtherIndex) const
{
	ensureAlwaysMsgf(PathFindingBlocks.IsValidIndex(SelfIndex) && PathFindingBlocks.IsValidIndex(OtherIndex), TEXT(""));

	const FTilemapPathFindingBlock SelfPathFindingBlock = PathFindingBlocks[SelfIndex];
	const FTilemapPathFindingBlock OtherPathFindingBlock = PathFindingBlocks[OtherIndex];

	if (FMath::Abs(SelfPathFindingBlock.Location.Z - OtherPathFindingBlock.Location.Z) < HeightSlowIncrement)
	{
		return 1;
	}
	if (FMath::Abs(SelfPathFindingBlock.Location.Z - OtherPathFindingBlock.Location.Z) < HeightBetweenLevel)
	{
		return FMath::Floor(
			FMath::Abs(SelfPathFindingBlock.Location.Z - OtherPathFindingBlock.Location.Z) / HeightSlowIncrement) + 1;
	}
	return 0;
}

void UTilemapAsset::AddEdgeBothWays(int32 Index, int32 EdgeIndex, int32 Cost)
{
	AddEdge(Index, EdgeIndex, Cost);
	AddEdge(EdgeIndex, Index, Cost);
}

void UTilemapAsset::AddEdge(int32 Index, int32 EdgeIndex, int32 Cost)
{
	ensureAlwaysMsgf(PathFindingBlocks.IsValidIndex(Index),
	                 TEXT("UTilemapAsset::AddEdge, [%d] is invalid index in PathFinding blocks"), Index);

	FTilemapPathFindingBlock& Block = PathFindingBlocks[Index];
	const int32 Flag = Block.EdgeArrayIndex.Find(EdgeIndex);
	if (Flag != INDEX_NONE)
	{
		Block.EdgeArray[Flag] = {EdgeIndex, Cost};
	}
	else
	{
		Block.EdgeArrayIndex.Add(EdgeIndex);
		Block.EdgeArray.Add({EdgeIndex, Cost});
	}
}

void UTilemapAsset::RemoveEdgeBothWays(int32 Index, int32 EdgeIndex)
{
	RemoveEdge(Index, EdgeIndex);
	RemoveEdge(EdgeIndex, Index);
}

void UTilemapAsset::RemoveCornerEdgeBothWays(int32 Index, int32 EdgeIndex)
{
	const int32 Row1 = Index / LevelSizeX;
	const int32 Row2 = EdgeIndex / LevelSizeX;

	const int32 Col1 = Index % LevelSizeX;
	const int32 Col2 = Index % LevelSizeX;

	if (Row1 > Row2)
	{
		if (Col1 - Col2 > 0)
		{
			RemoveEdgeBothWays(Index - 1, Index - LevelSizeX);
		}else if (Col1 - Col2 < 0)
		{
			RemoveEdgeBothWays(Index + 1, Index - LevelSizeX);
		}
	}else if (Row1 < Row2)
	{
		if (Col1 - Col2 > 0)
		{
			RemoveEdgeBothWays(Index + LevelSizeX, Index - 1);
		}else if (Col1 - Col2 < 0)
		{
			RemoveEdgeBothWays(Index + LevelSizeX, Index + 1);
		}
	}
}

void UTilemapAsset::RemoveEdge(int32 Index, int32 EdgeIndex)
{
	ensureAlwaysMsgf(PathFindingBlocks.IsValidIndex(Index),
	                 TEXT("UTilemapAsset::RemoveEdge, [%d] is invalid index in PathFinding blocks"), Index);

	FTilemapPathFindingBlock& Block = PathFindingBlocks[Index];
	const int32 Flag = Block.EdgeArrayIndex.Find(EdgeIndex);
	if (Flag == INDEX_NONE)
	{
		return;
	}
	Block.EdgeArray.RemoveAt(Flag);
	Block.EdgeArrayIndex.RemoveAt(Flag);
}

void UTilemapAsset::RemoveInValidEdge(int32 Index)
{
	ensureAlwaysMsgf(PathFindingBlocks.IsValidIndex(Index),
	                 TEXT("UTilemapAsset::RemoveInValidEdge, [%d] is invalid index in PathFinding blocks"), Index);

	FTilemapPathFindingBlock& Block = PathFindingBlocks[Index];
	for (int32 i = 0; i < Block.EdgeArrayIndex.Num(); ++i)
	{
		const int32 EdgeIndex = Block.EdgeArrayIndex[i];
		if (EdgeIndex > -1 && EdgeIndex < LevelSizeX * LevelSizeY * LevelSizeZ)
		{
			const FTilemapPathFindingBlock& EdgeBlock = PathFindingBlocks[EdgeIndex];
			int32 Distance = FMath::Abs(Block.Location.X - EdgeBlock.Location.X);
			if (Distance > GridSize)
			{
				RemoveEdge(Index, EdgeIndex);
			}
			Distance = FMath::Abs(Block.Location.Y - EdgeBlock.Location.Y);
			if (Distance > GridSize)
			{
				RemoveEdge(Index, EdgeIndex);
			}
		}
	}
}

void UTilemapAsset::SetEdgeCost(int32 Index, int32 EdgeIndex, int32 Cost)
{
	FTilemapPathFindingBlock& PathFindingBlock = PathFindingBlocks[Index];
 	const int32 Flag = PathFindingBlock.EdgeArrayIndex.Find(EdgeIndex);
	if (Flag != INDEX_NONE)
	{
		PathFindingBlock.EdgeArray[Flag].Cost = Cost;
	}
}
