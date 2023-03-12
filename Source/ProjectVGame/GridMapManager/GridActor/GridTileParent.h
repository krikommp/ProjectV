// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridActor.h"
#include "GridMapManager/GridMapStruct.h"
#include "GridTileParent.generated.h"

class USplineComponent;
class UStaticMesh;
class USplineMeshComponent;

USTRUCT(BlueprintType)
struct FGridEdgeCost
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cost")
	int32 NorthCost = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cost")
	int32 EastCost = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cost")
	int32 SouthCost = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cost")
	int32 WestCost = 99;
};

USTRUCT(BlueprintType)
struct FGridCustomEdge
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CostTo = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CostBack = INDEX_NONE;
};

/**
 * AGridTileParent
 *
 * 所有Tile的父类，用于处理这个Tile与其他邻边的交互关系
 */
UCLASS(Abstract)
class PROJECTVGAME_API AGridTileParent : public AGridActor
{
	GENERATED_BODY()
	friend class UGridMapFunctionLibrary;

public:
	AGridTileParent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ~AActor interface
	virtual void OnConstruction(const FTransform& Transform) override;
	// ~AActor interface

	
private:
	void MakeEdgeCostIntArray();
	
	void AddEdgeCostToEdgeCostArray(int32 Index, int32 Cost);

#if WITH_EDITOR
	void DisplayEdgeConnectLines();
	static TArray<FVector> DisplayBezierCurve(const FVector& StartLocation, const FVector& EndLocation);
#endif
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FGridEdgeCost EdgeCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	TArray<FGridCustomEdge> CustomEdges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	TArray<int32> BlockedEdges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	bool bShowConnectLine = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float SplineMeshLength = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category="Grid")
	TObjectPtr<USplineComponent> EdgeLinkSplineComponent;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshArray;

	UPROPERTY(EditAnywhere, Category="Grid")
	TObjectPtr<UStaticMesh> EdgeLinkMesh;

	UPROPERTY(EditAnywhere, Category="Grid")
	TObjectPtr<UStaticMesh> EdgeHeadLinkMesh;
	
	TArray<FStructIndexCost> EdgeCostStructArray;

	TArray<int32> EdgeIndexes;
};
