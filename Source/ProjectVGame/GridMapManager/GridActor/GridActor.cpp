// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActor.h"

#include "GridLogChannel.h"
#include "GridMapManager/GridMapFunctionLibrary.h"
#include "GridMapManager/GridMapManager.h"
#include "Kismet/GameplayStatics.h"


AGridActor::AGridActor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	CustomGridAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("CustomGridAnchor"));
	CustomGridAnchor->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = true;
}

void AGridActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TArray<AActor*> OuterActors;
	UGameplayStatics::GetAllActorsOfClass(this, AGridMapManager::StaticClass(), OuterActors);
	if (OuterActors.Num() <= 0)
	{
		UE_LOG(LogGrid, Error, TEXT("No GridMap instance in this level, you should place one！"));
		return;
	}
	if (OuterActors.Num() > 1)
	{
		UE_LOG(LogGrid, Warning, TEXT("More than one GridMap be found in this Level, and i will select the first one [%s]"), *GetNameSafe(OuterActors[0]));
	}
	GridMapManager = Cast<AGridMapManager>(OuterActors[0]);

	float OuterZ;
	UGridMapFunctionLibrary::VectorToIndex3DNative(GridMapManager, GetActorLocation(), TileIndex, OuterZ);
	OuterZ += ZOffset;

	if (GridMapManager->bSnapToCustomGrid && !bIgnoreSnapToCustomGrid)
	{
		FVector ActorLocation = GridMapManager->GetActorLocation();
		CustomGridAnchor->SetWorldLocation(FVector(ActorLocation.X + GridMapManager->VectorFieldArray[TileIndex].X, ActorLocation.Y + GridMapManager->VectorFieldArray[TileIndex].Y, OuterZ));
		if (bPlaceActorToAnchor)
		{
			SetActorLocation(CustomGridAnchor->GetComponentLocation());
			bPlaceActorToAnchor = false;
			ActorLocation = GridMapManager->GetActorLocation();
			CustomGridAnchor->SetWorldLocation(FVector(ActorLocation.X + GridMapManager->VectorFieldArray[TileIndex].X, ActorLocation.Y + GridMapManager->VectorFieldArray[TileIndex].Y, OuterZ));
		}
	}
}

void AGridActor::SpawnDefaultController()
{
	return;
}

int32 AGridActor::MeshToInstance(const AActor* Actor)
{
	return 0;
}

FVector AGridActor::GetVectorArray(int32 Index)
{
	return FVector::Zero();
}

int32 AGridActor::VectorToIndex(const FVector& Vector)
{
	return 0;
}

int32 AGridActor::GetTileIndex() const
{
	return TileIndex;
}
