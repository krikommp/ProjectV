#include "Tilemap3DTileMeshGenerator.h"

#include "GridTraceChannel.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Tilemap/TilemapAsset.h"

void FTilemap3DTileMeshGenerator::Setup(UTilemapAsset* InTilemapAsset, FTileMeshMap& TileMeshMap,
                                        FPreviewScene* PreviewScene, const UTileSet3DAsset* TileSet3DAsset)
{
	const int32 Count = InTilemapAsset->LevelSizeX * InTilemapAsset->LevelSizeY * InTilemapAsset->LevelSizeZ;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		FBlock& Block = InTilemapAsset->Blocks[Index];
		if (Block.MeshIndex != FName())
		{
			const FTileSet3DMesh* MeshSet = TileSet3DAsset->TileMeshSets.FindByPredicate([&](const FTileSet3DMesh& Item)
			{
				return Item.ID == Block.MeshIndex;
			});
			if (MeshSet != nullptr)
			{
				const int32 InstancedIndex = AddTileMesh({
					InTilemapAsset,
					PreviewScene,
					TileMeshMap,
					Block.MeshIndex,
					MeshSet->Mesh,
					Block.MeshTransform,
					Index,
					MeshSet->Material
				});
				Block.MeshInstancedIndex = InstancedIndex;
			}
		}
	}
}

int32 FTilemap3DTileMeshGenerator::AddTileMesh(const FAddTileMeshParams& Params)
{
	UInstancedStaticMeshComponent* MeshComponent;
	if (Params.InTileMeshMap.Contains(Params.ID))
	{
		MeshComponent = Params.InTileMeshMap[Params.ID];
	}
	else
	{
		MeshComponent = NewObject<UInstancedStaticMeshComponent>();
		MeshComponent->SetStaticMesh(Params.InMesh);
		MeshComponent->SetMaterial(0, Params.InMaterial);
		MeshComponent->SetCollisionResponseToChannel(WallTrace, ECR_Block);
		Params.InPreviewScene->AddComponent(MeshComponent, FTransform::Identity);
		Params.InTileMeshMap.Add(Params.ID, MeshComponent);
	}
	return MeshComponent->AddInstance(Params.Transform);
}
