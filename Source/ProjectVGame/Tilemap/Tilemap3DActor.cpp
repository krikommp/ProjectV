// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridTraceChannel.h"
#include "ProceduralMeshComponent.h"
#include "TilemapAsset.h"
#include "TileSet3DAsset.h"


ATilemap3DActor::ATilemap3DActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMeshComponent->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATilemap3DActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (TilemapAsset != nullptr)
	{
		ProceduralMeshComponent->SetMaterial(0, TilemapAsset->TileSet->Material);
		ProceduralMeshComponent->SetCollisionResponseToChannel(PathTrace, ECR_Block);
		ProceduralMeshComponent->CreateMeshSection(
			0,
			TilemapAsset->MeshData.Vertices,
			TilemapAsset->MeshData.Triangles,
			TilemapAsset->MeshData.Normals,
			TilemapAsset->MeshData.UV0,
			TilemapAsset->MeshData.Colors,
			TArray<FProcMeshTangent>(),
			true
		);
		InstanceMeshCompMap.Empty();
		for (UBlock* Block : TilemapAsset->Blocks)
		{
			if (Block->MeshIndex == FName())
				continue;
			UInstancedStaticMeshComponent* MeshComponent = nullptr;
			if (!InstanceMeshCompMap.Contains(Block->MeshIndex))
			{
				auto TileMeshSet = TilemapAsset->TileSet->TileMeshSets.FindByPredicate([&](const FTileSet3DMesh& Item)
				{
					return Item.ID ==Block->MeshIndex;
				});
				if (TileMeshSet != nullptr)
				{
					MeshComponent = Cast<UInstancedStaticMeshComponent>(
						AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, FTransform::Identity,
						                    false));
					MeshComponent->SetStaticMesh(TileMeshSet->Mesh);
					MeshComponent->SetMaterial(0, TileMeshSet->Material);
					MeshComponent->SetCollisionResponseToChannel(WallTrace, ECR_Block);
					InstanceMeshCompMap.Add(Block->MeshIndex, MeshComponent);
				}
			}
			else
			{
				MeshComponent = InstanceMeshCompMap[Block->MeshIndex];
			}
			Block->MeshInstancedIndex = MeshComponent->AddInstance(Block->MeshTransform);
		}
	}
}

void ATilemap3DActor::BeginPlay()
{
	Super::BeginPlay();
}
