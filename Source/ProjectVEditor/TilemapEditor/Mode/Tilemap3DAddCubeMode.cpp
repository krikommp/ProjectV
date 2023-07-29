#include "Tilemap3DAddCubeMode.h"

#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTerrainGenerator.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DAddCubeMode::FTilemap3DAddCubeMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0f)
{
}

void FTilemap3DAddCubeMode::EnterMode()
{
}

void FTilemap3DAddCubeMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			UEngineTypes::ConvertToTraceType(TilemapEditTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			if (ViewportClient.Pin()->GetCurrentFloor() != 0)
			{
				const int32 Index = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitResult.Location, ViewportClient.Pin()->GetCurrentFloor() - 1);
				FTilemap3DTileMeshGenerator::RemoveTileMesh(ViewportClient.Pin()->GetTilemapAsset(), ViewportClient.Pin()->GetTileMeshMap(), Index);
			}
			FTilemap3DTerrainGenerator::ModifyVoxel(ViewportClient.Pin()->GetTilemapAsset(), ViewportClient.Pin()->GetTerrainMesh(),
			                                        HitResult.Location,
			                                        ViewportClient.Pin()->GetTileCube(),
			                                        ViewportClient.Pin()->GetCurrentFloor(), ViewportClient.Pin()->GetTerrainMat(),
			                                        ViewportClient.Pin().Get());
		}
	}
}

void FTilemap3DAddCubeMode::ExitMode()
{
}
