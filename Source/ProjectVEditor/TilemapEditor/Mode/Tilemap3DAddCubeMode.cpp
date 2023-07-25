#include "Tilemap3DAddCubeMode.h"

#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTerrainGenerator.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DAddCubeMode::FTilemap3DAddCubeMode()
	: HitResultTraceDistance(10000.0f)
{
	EditMode = EEM_Cube_Append;
}

void FTilemap3DAddCubeMode::EnterMode()
{
}

void FTilemap3DAddCubeMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
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
			UEngineTypes::ConvertToTraceType(TilemapEditTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			if (ViewportClient->GetCurrentFloor() != 0)
			{
				const int32 Index = ViewportClient->GetTilemapAsset()->VectorToIndex(HitResult.Location, ViewportClient->GetCurrentFloor() - 1);
				FTilemap3DTileMeshGenerator::RemoveTileMesh(ViewportClient->GetTilemapAsset(), ViewportClient->GetTileMeshMap(), Index);
			}
			FTilemap3DTerrainGenerator::ModifyVoxel(ViewportClient->GetTilemapAsset(), ViewportClient->GetTerrainMesh(),
			                                        HitResult.Location,
			                                        ViewportClient->GetTileCube(),
			                                        ViewportClient->GetCurrentFloor(), ViewportClient->GetTerrainMat(),
			                                        ViewportClient);
		}
	}
}

void FTilemap3DAddCubeMode::ExitMode()
{
}
