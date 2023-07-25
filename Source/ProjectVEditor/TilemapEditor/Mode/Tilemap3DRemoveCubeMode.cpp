#include "Tilemap3DRemoveCubeMode.h"

#include "ProjectVEditor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTerrainGenerator.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DRemoveCubeMode::FTilemap3DRemoveCubeMode()
	: FTilemap3DBaseMode(), HitResultTraceDistance(10000.0f)
{
	EditMode = EEM_Cube_Remove;
}

void FTilemap3DRemoveCubeMode::EnterMode()
{
}

void FTilemap3DRemoveCubeMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
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
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			FTilemap3DTerrainGenerator::ModifyVoxel(ViewportClient->GetTilemapAsset(), ViewportClient->GetTerrainMesh(),
													HitResult.Location,
													FTileSet3DCube::EmptyBlock,
													ViewportClient->GetCurrentFloor(), ViewportClient->GetTerrainMat(),
													ViewportClient);
			const int32 Index = ViewportClient->GetTilemapAsset()->VectorToIndex(HitResult.Location, ViewportClient->GetCurrentFloor());
			FTilemap3DTileMeshGenerator::RemoveTileMesh(ViewportClient->GetTilemapAsset(), ViewportClient->GetTileMeshMap(), Index);
		}
	}
}

void FTilemap3DRemoveCubeMode::ExitMode()
{
}
