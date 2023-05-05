// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridMapManager.h"
#include "GridActor/GridTileParent.h"
#include "GridMapManager/GridMapStruct.h"
#include "GridMapFunctionLibrary.generated.h"

class AGridMapManager;
class UGridMapDataAsset;
class UDecalComponent;
class AGridTileParent;

/**
 * UGridMapFunctionLibrary
 *
 * 辅助创建地图数据
 */
UCLASS(Blueprintable, Category = "GridEditor|Map")
class PROJECTVGAME_API UGridMapFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Editor Scripting | DataAsset")
	static void CreateNewGridMapDataAsset(AGridMapManager* GridMapManager);

	UFUNCTION(BlueprintCallable, Category="GridMap|Create")
	static UDecalComponent* CreateGridMapDecalComponent(UMaterialInterface* DecalMaterial, FVector DecalSize,
	                                                    UWorld* World, AActor* Actor, float LifeSpan);

	UFUNCTION(BlueprintPure, Category="GridMap|Helper")
	static void VectorToIndex3DNative(AGridMapManager* GridMapManager, const FVector& Location, int32& Index, float& Z);

	UFUNCTION(BlueprintPure, Category="GridMap|Helper")
	static int32 VectorToIndex3D(AGridMapManager* GridMapManager, const FVector& Vector);

	UFUNCTION(BlueprintCallable, Category="GridMap|Helper")
	static void DisplayGridMapIndexAndEdge(AGridMapManager* GridMapManager);

	UFUNCTION(BlueprintCallable, Category="GridMap|Helper")
	static bool CompareClickLocation(AGridMapManager* GridMapManager, const FVector& ClickedLocation,
	                                 int32 ClickedIndex);

	UFUNCTION(BlueprintCallable, Category="GridMap|Helper")
	static void DisplayMoveRangeEdgeMarkers(AGridMapManager* GridMapManager,
	                                        int32 StartIndex,
	                                        const TArray<FStructPathFinding>& InCanMoveToArray,
	                                        const TArray<FStructPathFinding>& InIndexCanMoveToArray);

	UFUNCTION(BlueprintCallable, Category="GridMap|Helper")
	static void DisplayInsightRangeEdgeMarkers(AGridMapManager* GridMapManager,
	                                           const TArray<int32>& InTilesInSightArray,
	                                           const TArray<int32>& InRangeArray);

	template <typename T>
	static void SetArrayElement(T Item, TArray<T>& ItemArray, int32 Index)
	{
		if (ItemArray.Num() - 1 < Index)
		{
			ItemArray.SetNum(Index);
		}
		ItemArray.Insert(Item, Index);
	}

private:
	void static RemoveTileEdge(int32 TileIndex, int32 Edge, AGridMapManager* GridMapManager);

	void static RemoveTileEdgeBothWays(int32 Index1, int32 Index2, AGridMapManager* GridMapManager);

	void static SetEdgeCost(int32 TileIndex, int32 Edge, int32 NewCost, AGridMapManager* GridMapManager);

	void static RemoveEdgesOnEastAndWestSideOfGrid(int TileIndex, AGridMapManager* GridMapManager);

	void static RemoveEdgesOnNorthAndSouthSideOfGrid(int TileIndex, AGridMapManager* GridMapManager);

	void static BlockCornersInBetweenDiagonalTiles(int32 Index1, int32 Index2, AGridMapManager* GridMapManager);

	void static SetCollisionPlaneAndHeightmapBoxScaleAndLocation(AGridMapManager* GridMapManager, FVector& Location,
	                                                             float& ScaleX, float& ScaleY);

	FVector static IndexToVectorSquareGrid(int32 Index, AGridMapManager* GridMapManager);

	void static PopulateEdgeTileAndCost(AGridMapManager* GridMapManager);

	void static AddViewportTerrainToArrays(AGridMapManager* GridMapManager);

	void static AutoGeneratorHeightAndRotation(AGridMapManager* GridMapManager);

	void static CreateWallsOnGridEdges(AGridMapManager* GridMapManager);

	void static AutoCalcEdgeBaseOnHeight(AGridMapManager* GridMapManager);

	void static TraceForWallsFunction(AGridMapManager* GridMapManager);

	void static ConstructGridMapData(AGridMapManager* GridMapManager);

	void static PreGeneratorGridMap(AGridMapManager* GridMapManager);

	void static AddMultiLevelGridAtIndex(AGridMapManager* GridMapManager, int32 InIndex, const FVector& StartLocation);

	void static PrepareForCreateMultiLevelGrids(AGridMapManager* GridMapManager);

	TArray<int32> static GetAdjacentIndexes(AGridMapManager* GridMapManager, int32 Index, bool bDiagonal);

	int32 static GetEdgeCostFromZDifferent(AGridMapManager* GridMapManager, float ParentZ, int32 ChildIndex);

	void static AddEdgeBothWays(AGridMapManager* GridMapManager, int32 TileIndex, int32 Edge, int32 Cost);

	void static AddEdge(AGridMapManager* GridMapManager, int32 TileIndex, int32 Edge, int32 Cost);

	void static CollectAllTileParentOnGridMapAndAddToEdgeArray(AGridMapManager* GridMapManager);

	void static AddTileEdgesToEdgeArray(AGridMapManager* GridMapManager, AGridTileParent* Tile);

	void static SpawnEdgeMeshes(AGridMapManager* GridMapManager, int32 StartIndex,
	                            const TArray<FStructPathFinding>& InCanMoveToArray,
	                            UMaterialInterface* DecalMat, int32 Index);

	void static SpawnEdgeMeshes(AGridMapManager* GridMapManager,
	                            const TArray<int32>& InRangeArray,
	                            UMaterialInterface* DecalMat, int32 Index);

	void static SpawnEdgeDecalBetweenIndexes(AGridMapManager* GridMapManager,
	                                         int32 StartIndex,
	                                         const TArray<FStructPathFinding>& InCanMoveToArray, int32 OutsideIndex,
	                                         int32 InsideIndex, const FVector& DecalSize, float Rotation,
	                                         UMaterialInterface* DecalMat);

	void static SpawnEdgeDecalBetweenIndexes(AGridMapManager* GridMapManager,
	                                         const TArray<int32>& InRangeArray, int32 OutsideIndex,
	                                         int32 InsideIndex, const FVector& DecalSize, float Rotation,
	                                         UMaterialInterface* DecalMat);
};
