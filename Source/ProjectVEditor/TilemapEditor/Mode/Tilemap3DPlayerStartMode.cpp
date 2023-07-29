#include "Tilemap3DPlayerStartMode.h"

#include "GridTraceChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DPlayerStartMode::FTilemap3DPlayerStartMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0f)
{
}

void FTilemap3DPlayerStartMode::EnterMode()
{
}

void FTilemap3DPlayerStartMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
		if (HitResult.IsValidBlockingHit())
		{
			FVector HitLocation = HitResult.Location;
			HitLocation.Z -= ViewportClient.Pin()->GetTilemapAsset()->GridSize * 0.5f;
			const int32 Index = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitLocation);

			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient.Pin()->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = Index / (ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->
				LevelSizeY) * ViewportClient.Pin()->GetTilemapAsset()->GridSize + ViewportClient.Pin()->GetTilemapAsset()->GridSize;
			Transform.SetLocation(Location);

			ViewportClient.Pin()->GetTilemapAsset()->PlayerStart = Transform;
		}
	}
}

void FTilemap3DPlayerStartMode::ExitMode()
{
}
