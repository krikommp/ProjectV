#include "Tilemap3DRemoveMeshMode.h"

#include "GridTraceChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DRemoveMeshMode::FTilemap3DRemoveMeshMode()
	: FTilemap3DBaseMode(), HitResultTraceDistance(10000.0f)
{
	EditMode = EEM_Mesh_Remove;
}

void FTilemap3DRemoveMeshMode::EnterMode()
{
}

void FTilemap3DRemoveMeshMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
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
			FBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient->GetTilemapAsset()->Floors; ++z)
			{
				const int32 Index = BottomIndex + z * ViewportClient->GetTilemapAsset()->LevelSizeX * ViewportClient->GetTilemapAsset()->LevelSizeY;
				if (!ViewportClient->GetTilemapAsset()->Blocks.IsValidIndex(Index))
					continue;
				FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[Index];
				if (Block.MeshIndex != FName())
				{
					if (Block.MeshTransform.GetLocation().Z < HitResult.Location.Z)
					{
						BottomBlock = &Block;
					}
				}
			}

			if (BottomBlock != nullptr)
			{
				FTilemap3DTileMeshGenerator::RemoveTileMesh(ViewportClient->GetTilemapAsset(), ViewportClient->GetTileMeshMap(), *BottomBlock);
			}
		}
	}
}

void FTilemap3DRemoveMeshMode::ExitMode()
{
}
