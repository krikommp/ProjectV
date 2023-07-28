﻿#include "Tilemap3DRemoveMeshMode.h"

#include "GridTraceChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DRemoveMeshMode::FTilemap3DRemoveMeshMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0f)
{
}

void FTilemap3DRemoveMeshMode::EnterMode()
{
}

void FTilemap3DRemoveMeshMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			UBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient.Pin()->GetTilemapAsset()->Floors; ++z)
			{
				const int32 Index = BottomIndex + z * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeY;
				if (!ViewportClient.Pin()->GetTilemapAsset()->Blocks.IsValidIndex(Index))
					continue;
				UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index];
				if (Block->MeshIndex != FName())
				{
					if (Block->MeshTransform.GetLocation().Z < HitResult.Location.Z)
					{
						BottomBlock = Block;
					}
				}
			}

			if (BottomBlock != nullptr)
			{
				FTilemap3DTileMeshGenerator::RemoveTileMesh(ViewportClient.Pin()->GetTilemapAsset(), ViewportClient.Pin()->GetTileMeshMap(), BottomBlock);
			}
		}
	}
}

void FTilemap3DRemoveMeshMode::ExitMode()
{
}
