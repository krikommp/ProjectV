#include "Tilemap3DAddCubeMode.h"

#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTerrainGenerator.h"

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
