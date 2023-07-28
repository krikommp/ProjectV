#include "Tilemap3DRemoveCubeMode.h"

#include "ProjectVEditor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTerrainGenerator.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DRemoveCubeMode::FTilemap3DRemoveCubeMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0f)
{
}

void FTilemap3DRemoveCubeMode::EnterMode()
{
}

void FTilemap3DRemoveCubeMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			FTilemap3DTerrainGenerator::ModifyVoxel(ViewportClient.Pin()->GetTilemapAsset(), ViewportClient.Pin()->GetTerrainMesh(),
													HitResult.Location,
													&FTileSet3DCube::EmptyBlock,
													ViewportClient.Pin()->GetCurrentFloor(), ViewportClient.Pin()->GetTerrainMat(),
													ViewportClient.Pin().Get());
			const int32 Index = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitResult.Location, ViewportClient.Pin()->GetCurrentFloor());
			FTilemap3DTileMeshGenerator::RemoveTileMesh(ViewportClient.Pin()->GetTilemapAsset(), ViewportClient.Pin()->GetTileMeshMap(), Index);
		}
	}
}

void FTilemap3DRemoveCubeMode::ExitMode()
{
}
