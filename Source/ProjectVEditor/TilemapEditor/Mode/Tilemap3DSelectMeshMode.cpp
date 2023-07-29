﻿#include "Tilemap3DSelectMeshMode.h"

#include "GridTraceChannel.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DSelectMeshMode::FTilemap3DSelectMeshMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0), InstancedIndex(INDEX_NONE), BlockIndex(INDEX_NONE)
{
}

void FTilemap3DSelectMeshMode::EnterMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}

void FTilemap3DSelectMeshMode::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (EventArgs.Key == EKeys::LeftMouseButton && EventArgs.Event == IE_Pressed)
	{
		FViewportCursorLocation CursorLocation = ViewportClient.Pin()->GetCursorWorldLocationFromMousePos();

		FHitResult HitResult;
		const TArray<AActor*> IgnoreActor;
		UKismetSystemLibrary::LineTraceSingle(
			ViewportClient.Pin()->GetWorld(),
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
			const int32 BottomIndex = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitResult.Location, 0);
			const UBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient.Pin()->GetTilemapAsset()->Floors; ++z)
			{
				const int32 Index = BottomIndex + z * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeY;
				if (!ViewportClient.Pin()->GetTilemapAsset()->Blocks.IsValidIndex(Index))
					continue;
				const UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index];
				if (Block->MeshIndex != FName())
				{
					if (Block->MeshTransform.GetLocation().Z < HitResult.Location.Z)
					{
						BottomBlock = Block;
						BlockIndex = Index;
					}
				}
			}

			if (BottomBlock != nullptr)
			{
				InstancedIndex = BottomBlock->MeshInstancedIndex;
				MeshComponent = ViewportClient.Pin()->GetTileMeshMap()[BottomBlock->MeshIndex];
				//MeshComponent->SelectInstance(true, BottomBlock->MeshInstancedIndex);
			}
		}
	}
	if (EventArgs.Key == EKeys::R && EventArgs.Event == IE_Pressed)
	{
		if (MeshComponent != nullptr && InstancedIndex != INDEX_NONE && ViewportClient.Pin()->GetTilemapAsset()->Blocks.IsValidIndex(BlockIndex))
		{
			FTransform Transform;
			MeshComponent->GetInstanceTransform(InstancedIndex, Transform);
			FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f));
			Transform.SetRotation(Rotation * Transform.GetRotation());
			MeshComponent->UpdateInstanceTransform(InstancedIndex, Transform, false, true, false);

			UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[BlockIndex];
			Block->MeshTransform = Transform;
		}
	}
}

void FTilemap3DSelectMeshMode::ExitMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}
