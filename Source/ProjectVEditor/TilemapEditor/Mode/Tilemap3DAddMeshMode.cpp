#include "Tilemap3DAddMeshMode.h"

#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

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
			EDrawDebugTrace::Persistent,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			const int32 Index = ViewportClient->GetTilemapAsset()->VectorToIndex(HitResult.Location, 0);
			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = HitResult.Location.Z;
			Transform.SetLocation(Location);
			ViewportClient->AddMesh(ViewportClient->GetTileMesh().ID, ViewportClient->GetTileMesh().Mesh, Transform, Index);
		}
	}
}
