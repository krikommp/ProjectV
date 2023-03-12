// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTileParent.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GridMapManager/GridMapFunctionLibrary.h"
#include "GridMapManager/GridMapManager.h"
#include "Kismet/KismetMathLibrary.h"

AGridTileParent::AGridTileParent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	EdgeLinkSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("EdgeLinkSplineComponent"));
	
	CustomEdges.Empty();
	BlockedEdges.Empty();
	SplineMeshArray.Empty();
}

void AGridTileParent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (GridMapManager == nullptr)  return;
	EdgeCostStructArray.Empty();
	EdgeIndexes.Empty();
	MakeEdgeCostIntArray();

#if WITH_EDITOR
	if (bShowConnectLine)
	{
		DisplayEdgeConnectLines();
	}
#endif
}

void AGridTileParent::MakeEdgeCostIntArray()
{
	AddEdgeCostToEdgeCostArray(TileIndex - GridMapManager->GridSizeX, EdgeCost.NorthCost);
	AddEdgeCostToEdgeCostArray(TileIndex + 1, EdgeCost.EastCost);
	AddEdgeCostToEdgeCostArray(TileIndex + GridMapManager->GridSizeX, EdgeCost.SouthCost);
	AddEdgeCostToEdgeCostArray(TileIndex -1, EdgeCost.WestCost);

	EdgeIndexes.Empty();

	for (const auto& Edge : EdgeCostStructArray)
	{
		EdgeIndexes.Add(Edge.Index);
	}
}

void AGridTileParent::AddEdgeCostToEdgeCostArray(int32 Index, int32 Cost)
{
	if (Cost <= 0) return;

	EdgeCostStructArray.Add({ Index, Cost });
}

#if WITH_EDITOR
void AGridTileParent::DisplayEdgeConnectLines()
{
	ensureAlwaysMsgf(GridMapManager, TEXT("GridMapManager is nullptr"));

	for (const auto& SplineMesh : SplineMeshArray)
	{
		SplineMesh->DestroyComponent();
	}
	SplineMeshArray.Empty();
	
	const FVector TileLocation = GridMapManager->IndexToVectorOnGrid(TileIndex, ZOffset);
	for (const auto& CustomEdge : CustomEdges)
	{
		if (CustomEdge.Index == INDEX_NONE) break;
		
		const FVector EdgeLocation = GridMapManager->IndexToVectorOnGrid(CustomEdge.Index, ZOffset);

		if (CustomEdge.CostTo != INDEX_NONE)
		{
			const TArray<FVector> Locations = DisplayBezierCurve(TileLocation, EdgeLocation);
			EdgeLinkSplineComponent->SetSplinePoints(Locations, ESplineCoordinateSpace::World, true);
			for (int32 Index = 0; Index < Locations.Num(); ++Index)
			{
				EdgeLinkSplineComponent->SetSplinePointType(Index, ESplinePointType::Linear, true);
			}
			double Remainder = 0.0f;
			const int32 EndPoint = UKismetMathLibrary::FMod64(EdgeLinkSplineComponent->GetSplineLength(), SplineMeshLength, Remainder);

			for (int32 Index = 1; Index <= EndPoint; ++Index)
			{
				FTransform SplineTransform;
				USplineMeshComponent* SplineMeshComponent = Cast<USplineMeshComponent>(this->AddComponentByClass(USplineMeshComponent::StaticClass(), true, SplineTransform, false));
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				if (Index == EndPoint)
				{
					SplineMeshComponent->SetStaticMesh(EdgeHeadLinkMesh);
				}else
				{
					SplineMeshComponent->SetStaticMesh(EdgeLinkMesh);
				}
				SplineMeshComponent->SetStartAndEnd(
					EdgeLinkSplineComponent->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World),
					EdgeLinkSplineComponent->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World) *SplineMeshLength,
					EdgeLinkSplineComponent->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength, ESplineCoordinateSpace::World),
					EdgeLinkSplineComponent->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength, ESplineCoordinateSpace::World) * SplineMeshLength,
					true);
				SplineMeshArray.Add(SplineMeshComponent);
			}
		}

		if (CustomEdge.CostBack != INDEX_NONE)
		{
			const TArray<FVector> Locations = DisplayBezierCurve(EdgeLocation, TileLocation);
			EdgeLinkSplineComponent->SetSplinePoints(Locations, ESplineCoordinateSpace::World, true);
			for (int32 Index = 0; Index < Locations.Num(); ++Index)
			{
				EdgeLinkSplineComponent->SetSplinePointType(Index, ESplinePointType::Linear, true);
			}
			double Remainder = 0.0f;
			const int32 EndPoint = UKismetMathLibrary::FMod64(EdgeLinkSplineComponent->GetSplineLength(), SplineMeshLength, Remainder);

			for (int32 Index = 1; Index <= EndPoint; ++Index)
			{
				FTransform SplineTransform;
				USplineMeshComponent* SplineMeshComponent = Cast<USplineMeshComponent>(this->AddComponentByClass(USplineMeshComponent::StaticClass(), true, SplineTransform, false));
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				if (Index == EndPoint)
				{
					SplineMeshComponent->SetStaticMesh(EdgeHeadLinkMesh);
				}else
				{
					SplineMeshComponent->SetStaticMesh(EdgeLinkMesh);
				}
				SplineMeshComponent->SetStartAndEnd(
					EdgeLinkSplineComponent->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World),
					EdgeLinkSplineComponent->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength, ESplineCoordinateSpace::World) *SplineMeshLength,
					EdgeLinkSplineComponent->GetLocationAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength, ESplineCoordinateSpace::World),
					EdgeLinkSplineComponent->GetDirectionAtDistanceAlongSpline(Index * SplineMeshLength + SplineMeshLength, ESplineCoordinateSpace::World) * SplineMeshLength,
					true);
				SplineMeshArray.Add(SplineMeshComponent);
			}
		}
	}
}

TArray<FVector> AGridTileParent::DisplayBezierCurve(const FVector& StartLocation, const FVector& EndLocation)
{
	constexpr float Step = 0.05f;
	TArray<FVector> Result;
	FVector TmpLocation = (StartLocation + EndLocation) / 2.0f;
	TmpLocation += { 0.0f, 0.0f, 600.0f };
	for (float t = 0.0f; t < 1; t+=Step)
	{
		FVector ResLocation;
		ResLocation.X = (1 - t) * (1 - t) * StartLocation.X + 2 * t * (1 - t) * TmpLocation.X + t * t * EndLocation.X;
		ResLocation.Y = (1 - t) * (1 - t) * StartLocation.Y + 2 * t * (1 - t) * TmpLocation.Y + t * t * EndLocation.Y;
		ResLocation.Z = (1 - t) * (1 - t) * StartLocation.Z + 2 * t * (1 - t) * TmpLocation.Z + t * t * EndLocation.Z;
		Result.Add(ResLocation);
	}
	return Result;
}
#endif
