#include "Tilemap3DSelectMeshMode.h"

#include "GridTraceChannel.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DSelectMeshMode::FTilemap3DSelectMeshMode()
	:HitResultTraceDistance(10000.0)
{
	EditMode = EEM_Mesh_Select;
}

void FTilemap3DSelectMeshMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
	const FInputKeyEventArgs& EventArgs)
{
	CHECK_EDITMODE(ViewportClient)
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
			EDrawDebugTrace::Persistent,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			const int32 BottomIndex = ViewportClient->GetTilemapAsset()->VectorToIndex(HitResult.Location, 0);
			const FBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient->GetTilemapAsset()->Floors; ++z)
			{
				const FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[BottomIndex + z * ViewportClient->GetTilemapAsset()->LevelSizeX * ViewportClient->GetTilemapAsset()->LevelSizeY];
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
				UInstancedStaticMeshComponent* MeshComponent = ViewportClient->GetTileMeshMap()[BottomBlock->MeshIndex];
				MeshComponent->SelectInstance(true, BottomBlock->MeshInstancedIndex);
			}
		}
	}
}
