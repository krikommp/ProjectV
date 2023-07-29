#include "Tilemap3DSelectMeshMode.h"

#include "GridTraceChannel.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DSelectMeshMode::FTilemap3DSelectMeshMode()
	:HitResultTraceDistance(10000.0), InstancedIndex(INDEX_NONE), BlockIndex(INDEX_NONE)
{
	EditMode = EEM_Mesh_Select;
}

void FTilemap3DSelectMeshMode::EnterMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}

void FTilemap3DSelectMeshMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
                                        const FInputKeyEventArgs& EventArgs)
{
	if (EventArgs.Key == EKeys::LeftMouseButton && EventArgs.Event == IE_Pressed)
	{
		FViewportCursorLocation CursorLocation = ViewportClient->GetCursorWorldLocationFromMousePos();

		FHitResult HitResult;
		const TArray<AActor*> IgnoreActor;
		UKismetSystemLibrary::LineTraceSingle(
			ViewportClient->GetWorld(),
			CursorLocation.GetOrigin(),
			CursorLocation.GetOrigin() + CursorLocation.GetDirection() * HitResultTraceDistance,
			UEngineTypes::ConvertToTraceType(WallTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			const int32 BottomIndex = ViewportClient->GetTilemapAsset()->VectorToIndex(HitResult.Location, 0);
			const FBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient->GetTilemapAsset()->Floors; ++z)
			{
				const int32 Index = BottomIndex + z * ViewportClient->GetTilemapAsset()->LevelSizeX * ViewportClient->GetTilemapAsset()->LevelSizeY;
				if (!ViewportClient->GetTilemapAsset()->Blocks.IsValidIndex(Index))
					continue;
				const FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[Index];
				if (Block.MeshIndex != FName())
				{
					if (Block.MeshTransform.GetLocation().Z < HitResult.Location.Z)
					{
						BottomBlock = &Block;
						BlockIndex = Index;
					}
				}
			}

			if (BottomBlock != nullptr)
			{
				InstancedIndex = BottomBlock->MeshInstancedIndex;
				MeshComponent = ViewportClient->GetTileMeshMap()[BottomBlock->MeshIndex];
				//MeshComponent->SelectInstance(true, BottomBlock->MeshInstancedIndex);
			}
		}
	}
	if (EventArgs.Key == EKeys::R && EventArgs.Event == IE_Pressed)
	{
		if (MeshComponent != nullptr && InstancedIndex != INDEX_NONE && ViewportClient->GetTilemapAsset()->Blocks.IsValidIndex(BlockIndex))
		{
			FTransform Transform;
			MeshComponent->GetInstanceTransform(InstancedIndex, Transform);
			FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f));
			Transform.SetRotation(Rotation * Transform.GetRotation());
			MeshComponent->UpdateInstanceTransform(InstancedIndex, Transform, false, true, false);

			FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[BlockIndex];
			Block.MeshTransform = Transform;
		}
	}
}

void FTilemap3DSelectMeshMode::ExitMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}
