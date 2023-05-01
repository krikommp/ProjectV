// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPawn.h"
#include "GridMapStruct.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/SplineComponent.h"

#include "GridMapManager.generated.h"

class APostProcessVolume;
class UStaticMesh;
class UStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UBoxComponent;
class UInstancedStaticMeshComponent;
class UMaterialInterface;
class UDecalComponent;
class USplineComponent;
class AGridChessPiece;
class USplineMeshComponent;
class UTextRenderComponent;
class AGridTileParent;

/**
 * AGridMapManager
 *
 * 项目最基础的战旗网格
 */
UCLASS(Config=Game, meta=(ToolTip = "The base GridMapManager in this project"))
class PROJECTVGAME_API AGridMapManager : public AModularPawn, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

	friend class UGridMapFunctionLibrary;
	friend class UGridMapStateComponent;
	friend class UGridMapWarFogComponent;

public:
	AGridMapManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ~AActor interface
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	// ~AActor interface

	FVector IndexToVectorOnGrid(int32 InIndex, float ZOffset);

	FORCEINLINE const TArray<FVector>& GetPathVectorArray() const { return PathVectorArray; }
	FORCEINLINE const TArray<int32>& GetPathIndexArray() const { return PathIndexArray; }
	FORCEINLINE const TArray<int32>& GetRangeArray() const { return RangeArray; }
	FORCEINLINE const TArray<FStructRange>& GetTileInRangeArray() const { return TilesInRangeArray; }
	FORCEINLINE const TArray<int32>& GetDiscoverableTileIndex() const { return DiscoverableTileIndexArray; }
	FORCEINLINE const TArray<FStructPathFinding>& GetIndexCanMove() const { return IndexCanMoveToArray; }

	/**
	 * @brief 是否支持斜角移动，即八方向移动
	 * @return ture: 支持八方向移动；false: 支持四方向移动
	 */
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDiagonalMovement() const { return bDiagonalMovement; }

	// 根据输入的当前位置和移动范围大小，计算出可移动到的Tiles
	// 通过指定 DisplayTiles 来决定是否显示出可移动范围
	UFUNCTION(BlueprintCallable, Category="Grid|Path Finding")
	TArray<int32> PathFinding(int32 InStartIndex, int32 InMoveRange, int32 InMaxMoveRange, bool bExcludeFriendly,
	                          bool bContinueFromLastPathfinding, bool bShowStartIndex = false);

	// 查找所有可视范围网格
	UFUNCTION(BlueprintCallable, Category="Grid|Path Finding")
	const TArray<int32>& FindTilesInRange(int32 StartIndex, int32 InRange, bool bCheckVisibility, bool bFindOnlyPawns,
	                                      float MaxZDifference, int32 MinimumRange,
	                                      bool bExcludeFriendly);

	// 根据发起对象，在可视范围内查找是否有满足距离条件的棋子对象
	UFUNCTION(BlueprintCallable, Category="Grid|Path Finding")
	void FindChessPieceInRange(const TArray<FStructRange>& InTileInRangeArray,
	                           AGridChessPiece* InstigatorChessPiece, int32 Distance,
	                           TArray<int32>& OutTilesInsightArray, TArray<int32>& OutRangeArrat);

	// 根据输入位置索引，创建一条可到达的Path,并决定是否渲染出路径
	UFUNCTION(BlueprintCallable, Category="Grod|Path Finding")
	bool FindPathWithinPathfindingArray(int32 IndexPathEnd, bool bCreatePathSpline, bool bDisplayPath,
	                                    bool bDisplayPathAsSpline, int32 StopFromTarget = 0);

	// 计算两个索引之间的距离
	UFUNCTION(BlueprintCallable, Category="Grid|Path Finding")
	int32 FindDistanceInTilesBetweenIndexes(int32 Index1, int32 Index2);

	// 显示输入的TileIndex数组对应的Tile对象
	UFUNCTION(BlueprintCallable, Category="Grid|Decal")
	void DisplayRangeMarkers(const TArray<int32>& InTileIndexArray, UMaterialInterface* DecalMaterial,
	                         bool bOverrider = false);

	// 判断Tile是否可以通过， true：不可通过， false: 可通过
	UFUNCTION(BlueprintCallable, Category="Grid|Utils")
	bool IsTileImpassable(int32 Index);

	// 获取鼠标点击的位置和索引
	UFUNCTION(BlueprintCallable, Category="Grid|Path Finding")
	bool GetHitLocationAndIndex(APlayerController* InController, int32& OutIndex, FVector& OutLocation);

	// 清理所有贴花
	UFUNCTION(BlueprintCallable, Category="Grid|Decal")
	void ClearAllInRangeMeshMarkers();

	// 设置贴花可见性
	UFUNCTION(BlueprintCallable, Category="Grid|Decal")
	void SetMarkerVisibility(bool bVisibility, UDecalComponent* DecalComponent);

	// 设置贴花位置
	UFUNCTION(BlueprintCallable, Category="Grid|Decal")
	void SetMarkerLocationUseIndex(int32 Index, UDecalComponent* DecalComponent);

	// 设置贴花位置
	UFUNCTION(BlueprintCallable, Category="Grid|Decal")
	void SetMarkerLocationUseLocation(const FVector& Location, UDecalComponent* DecalComponent);

	// 清理路径
	UFUNCTION(BlueprintCallable, Category="Grid|Decal")
	void RemoveDisplayedPath();

	UFUNCTION(BlueprintPure, Category="Grid|Path Finding", meta=(DisplayName="GetTileInRangeArray"))
	FORCEINLINE TArray<FStructRange> K2_GetTileInRangeArray() const { return TilesInRangeArray; }

	UFUNCTION(BlueprintPure, Category="Grid|Path Finding", meta=(DisplayName="GetDiscoverableTileIndex"))
	FORCEINLINE TArray<int32> K2_GetDiscoverableTileIndex() const { return DiscoverableTileIndexArray; }

	UFUNCTION(BlueprintPure, Category="Grid|Path Finding", meta=(DisplayName="GetReachablePawnsArray"))
	FORCEINLINE TArray<int32> K2_GetReachablePawnsArray() const { return ReachablePawnsArray; }

	UFUNCTION(BlueprintPure, Category="Grid|Path Finding", meta=(DisplayName="GetIndexCanMove"))
	TArray<int32> K2_GetIndexCanMove() const;

	// 检查 OpenList 中所有的 Tile, 如果可以移动到，那么就加入到合适的数组中，等待下一次移动
	void SearchAndAddAdjacentTiles(TArray<FStructPathFinding>& InDelayedSpiltPathfindingList,
	                               TArray<FStructPathFinding>& InOpenList,
	                               TArray<FStructPathFinding>& InOpenListChildren,
	                               TArray<FStructPathFinding>& InDelaySearchList,
	                               TArray<FStructPathFinding>& OutCanMoveToArray,
	                               TArray<int32>& OutReachablePawnsArray,
	                               int32 InMaxMove,
	                               int32 InCurrentSearchStep,
	                               bool bShowStartIndex, int32 StartIndex, bool bExcludeFriendly);

	// 根据输入位置索引，创建一条可到达的Path
	const TArray<int32>& FindPathToIndex(const TArray<FStructPathFinding>& InCanMoveToArray, int32 InEndIndex,
	                                     int32 StopFromTarget);

	// 根据输入位置索引及范围查找到所有可以被找到的Tiles， 需要进行下一步判断
	void GetIndexesInRange(int32 StartIndex, int32 Range);

	// 判断 Index 到 TargetIndex 是否可见
	void CheckIfTileIsVisibleFromOtherTile(int32 Index, int32 TargetIndex, bool bFindOnlyPawns, float MaxZDifference,
	                                       int32 Distance, int32 MinimumRange, bool bCheckVisibility,
	                                       bool bExcludeFriendly);

	void PathFinding_Internal(int32 InStartIndex, int32 InMoveRange, int32 InMaxMoveRange, bool bExcludeFriendly,
	                          bool bContinueFromLastPathfinding, bool bShowStartIndex,
	                          OUT int32& OutCurrentSearchStep,
	                          OUT TArray<FStructPathFinding>& OutCanMoveToArray,
	                          OUT TArray<FStructPathFinding>& OutIndexCanMoveToArray,
	                          OUT TArray<int32>& OutTileIndexes, OUT TArray<int32>& OutReachablePawnsArray);

private:
	void DisplayTileIndexesInternal();
	void DisplayTileEdgesInternal();

public:
	// 初始化可视组件
	UFUNCTION(BlueprintCallable, Category="Grid|Visible")
	void InitializeVisibleComponent() const;

	// 初始化场景贴画可
	UFUNCTION(BlueprintCallable, Category="Grid|Visible")
	void InitializeDecalAndPathComponent();

	// 计算格子贴画的大小
	void ScaleDecalRelativeToDefaultTile(UDecalComponent* DecalComponent) const;

	// 创建PathMesh
	void CreateSplinePath(const TArray<int32>& InPathIndexArray);

	void DisplayPathAsSpline();

	void ResetPathfindingArraysAndClearMeshes();

	// 清理Path
	void DestroyAndClearSplinePath();

	UPROPERTY(BlueprintReadWrite, Category="Unit")
	TArray<AGridChessPiece*> PawnArray;

	/**
	* @brief 碰撞箱组件
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UStaticMeshComponent* CollisionPlane;

	/**
	 * @brief 可视化高度范围
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UBoxComponent* Heightmap;

	/**
	 * @brief 网格组件，可视化地形
	 */
	UPROPERTY(BlueprintReadWrite, Category="Component")
	UInstancedStaticMeshComponent* DefaultTileInstance;

	/**
	 * @brief 默认地形模型
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Resources")
	UStaticMesh* DefaultStaticMesh;

	/**
	 * @brief 用于显示边缘方向的箭头模型
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Resources")
	UStaticMesh* EdgeArrowStaticMesh;

	/**
	 * @brief 贴花与下面的Tile的高度差是多少
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GridOptions")
	float TileInRangeZOffset;

	// 当前被实例化的Decal
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDecalComponent>> CurrentDecalsArray;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="点击地面显示的贴花材质"))
	TObjectPtr<UMaterialInterface> HoverMarkerDecalMaterial;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="当前棋子显示的贴花材质"))
	TObjectPtr<UMaterialInterface> CurrentMarkerDecalMaterial;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="目标位置显示的贴花材质"))
	TObjectPtr<UMaterialInterface> TargetMarkerDecalMaterial;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="寻路模型"))
	TObjectPtr<UStaticMesh> PathMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="移动范围贴花材质"))
	TObjectPtr<UMaterialInterface> TileInMoveRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="移动边缘贴花材质"))
	TObjectPtr<UMaterialInterface> TileInMoveRangeEdgeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="可视范围贴花材质"))
	TObjectPtr<UMaterialInterface> TileInSightRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="可视边缘贴花材质"))
	TObjectPtr<UMaterialInterface> TileInSightRangeEdgeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="攻击范围贴花材质"))
	TObjectPtr<UMaterialInterface> TileInAttackRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="治疗范围贴花材质"))
	TObjectPtr<UMaterialInterface> TileInHealingRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="增益技能范围贴花材质"))
	TObjectPtr<UMaterialInterface> TileInBuffRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="寻路Spline模型"))
	TObjectPtr<UStaticMesh> SplinePathMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Resources",
		meta=(AllowPrivateAccess="true", ToolTip="寻路Spline材质"))
	TObjectPtr<UMaterialInterface> PathMaterial;

	/**
	 * @brief 地图大小
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	int32 GridSizeX = 0;

	/**
	 * @brief 地图大小
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	int32 GridSizeY = 0;

	/**
	 * @brief Spline 距离地面高度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float SplineHeight = 0.7f;

	/**
	 * @brief 地图最高范围
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float MaxGridHeight = 0.0f;

	/**
	 * @brief 地图最低范围
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float MinGridHeight = 0.0f;

	/**
	 * @brief 需要消耗额外行动力的高度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float HeightSlowIncrement = 100.0f;

	/**
	 * @brief 地图最高层级
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	int32 MaxLevels = 5;

	/**
	 * @brief 无法通过的高度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float HeightImpassableCutoff = 170.0f;

	// 可见到其他棋子的高度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float VisibilityTraceHeight = 170.0f;

	/**
	 * @brief 障碍的检测高度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float TraceForWallsHeight = 100.f;

	/**
	 * @brief 每层之前的高度差
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	float HeightBetweenLevels = 200.0f;

	/**
	 * @brief 是否显示默认地形
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bShowDefaultTile = true;

	/**
	 * @brief 是否显示地形碰撞体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bShowCollision = true;

	/**
	 * @brief 是否显示范围盒子
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bShowHeightmap = true;

	/**
	 * @brief 是否将Unit位置和Anchor同步
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bSnapToCustomGrid = true;

	// 自动生成的 Plane 是否可以行走
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bCollisionPlaneWalkable = true;

	// 显示Tile索引
	// 只在Editor模式下有效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bDisplayTileIndex = true;

	// 显示边界
	// 只在Editor模式下生效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|GridOptions")
	bool bDisplayTileEdge = true;

	/**
	 * @brief 地图大小
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	int32 GridSizeZ = 1;

	/**
	 * @brief 单位格子大小
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	float TileBoundsX = 100;

	/**
	 * @brief 单位格子大小
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	float TileBoundsY = 100;

	/**
	 * @brief 记录碰撞箱位置
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	FVector CollisionLocation = FVector(0, 0, 0);

	/**
	 * @brief 记录碰撞箱大小
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	FVector CollisionScale = FVector(0, 0, 0);

	/**
	 * @brief 记录Heightmap位置
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	FVector HeightmapLocation = FVector(0, 0, 0);

	/**
	 * @brief 记录Heightmap大小
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grid|Hide Property")
	FVector HeightmapScale = FVector(0, 0, 0);

	/**
	 * @brief 每个Tile位置
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Grid Size Array")
	TArray<FVector> VectorFieldArray;

	/**
	 * @brief 地图数组，记录当前寻路可以到达的tile
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Grid Size Array")
	TArray<FStructPathFinding> CanMoveToArray;

	/**
	 * @brief 只包含寻路路径的Tile队列，CanMoveToArray包含了整个地图，在只希望获取到路径的地方使用该变量
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Hide Property")
	TArray<FStructPathFinding> IndexCanMoveToArray;

	/**
	 * @brief  记录该单位到可见位置范围数组
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Hide Property")
	TArray<int32> RangeArray;

	/**
	 * @brief 记录该单位所有可见位置的数组
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Hide Property")
	TArray<int32> TilesInSightArray;

	/**
	 * @brief 地图数组，记录当前寻路可以到达的tile
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Grid Size Array")
	TArray<FStructEdgeArray> EdgeArray;

	/**
	 * @brief 地图数组，记录每一块Tile相邻索引
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Grid sized arrays")
	TArray<FStructIntArray> EdgeArrayInteger;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Grid sized arrays")
	TArray<TObjectPtr<AGridTileParent>> TileParents;

	/**
	 * @brief 记录可见范围的贴图
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Grid|Visible")
	TObjectPtr<UTexture2D> VisibleRangeTexture;

	// Hover Comp
	UPROPERTY(BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<UDecalComponent> HoverMarkerDecal;

	UPROPERTY(BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<UDecalComponent> CurrentMarkerDecal;

	UPROPERTY(BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<UDecalComponent> TargetMarkerDecal;

	UPROPERTY(BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<UInstancedStaticMeshComponent> Path;

	UPROPERTY(BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<USplineComponent> PathSpline;

	UPROPERTY(BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<USplineComponent> DisplaySpline;

protected:
	// 当前可以到达的所有Unit位置索引
	TArray<int32> ReachablePawnsArray;

	// 路径索引队列
	TArray<int32> PathIndexArray;

	// 记录寻路过程中造成的消耗
	int32 CurrentSearchStep;

	// 所有需要被检查可见性的数组
	TArray<FStructRange> TilesInRangeArray;

	// 记录被发现的位置索引
	TArray<int32> DiscoverableTileIndexArray;

private:
	// 每个Path索引对应的位置信息
	TArray<FVector> PathVectorArray;

	// 每格贴画大小
	UPROPERTY(Transient)
	FVector DecalSizeSquare;

	UPROPERTY(Transient)
	float SplineMeshLength;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshArray;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextRenderComponent>> DisplayIndexesTextRenderArray;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextRenderComponent>> DisplayEdgeCostTextRenderArray;

	UPROPERTY(Transient)
	TObjectPtr<UInstancedStaticMeshComponent> DisplayEdgeInstancedMeshComponent;

	UPROPERTY(Config)
	bool bDiagonalMovement = true;
};
