#include "Tilemap3DAddMeshMode.h"

#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DAddMeshMode::FTilemap3DAddMeshMode()
	: HitResultTraceDistance(10000.0f)
{
	EditMode = EEM_Mesh_Append;
}

void FTilemap3DAddMeshMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
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
			UEngineTypes::ConvertToTraceType(PathTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			FVector HitLocation = HitResult.Location;
			HitLocation.Z -= ViewportClient->GetTilemapAsset()->GridSize * 0.5f;
			const int32 Index = ViewportClient->GetTilemapAsset()->VectorToIndex(HitLocation);
			
			if (ViewportClient->GetTilemapAsset()->Blocks[Index].MeshIndex != FName())
				return;
			
			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = Index / (ViewportClient->GetTilemapAsset()->LevelSizeX * ViewportClient->GetTilemapAsset()->
				LevelSizeY) * ViewportClient->GetTilemapAsset()->GridSize + ViewportClient->GetTilemapAsset()->GridSize;
			Transform.SetLocation(Location);

			FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[Index];
			Block.MeshIndex = ViewportClient->GetTileMesh().ID;
			Block.MeshTransform = Transform;
			
			Block.MeshInstancedIndex = FTilemap3DTileMeshGenerator::AddTileMesh({
				ViewportClient->GetTilemapAsset(),
				ViewportClient->GetPreviewScene(),
				ViewportClient->GetTileMeshMap(),
				Block.MeshIndex,
				ViewportClient->GetTileMesh().Mesh,
				Transform,
				Index,
				ViewportClient->GetTileMesh().Material
			});
		}
	}
}
