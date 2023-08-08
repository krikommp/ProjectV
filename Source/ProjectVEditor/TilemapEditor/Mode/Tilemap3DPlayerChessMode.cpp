#include "Tilemap3DPlayerChessMode.h"

#include "GridTraceChannel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorSettings.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DPlayerChessMode::FTilemap3DPlayerChessMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0)
{
}

void FTilemap3DPlayerChessMode::EnterMode()
{
}

void FTilemap3DPlayerChessMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			FVector HitLocation = HitResult.Location;
			HitLocation.Z -= ViewportClient.Pin()->GetTilemapAsset()->GridSize * 0.5f;
			const int32 Index = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitLocation);

			if (ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->MeshIndex != FName())
				return;

			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient.Pin()->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = Index / (ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->
				LevelSizeY) * ViewportClient.Pin()->GetTilemapAsset()->GridSize + ViewportClient.Pin()->GetTilemapAsset()->GridSize;
			Transform.SetLocation(Location);

			UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();

			const FVector DecalSize = { 95.0f, 45.0f, 45.0f };
			//UGameplayStatics::SpawnDecalAtLocation(ViewportClient.Pin()->GetPreviewScene()->GetWorld(), Settings->DefaultPlayerChessRangeMat.LoadSynchronous(), DecalSize, Location, {90.0f, 0.0f, 0.0f});
		}
	}
}

void FTilemap3DPlayerChessMode::ExitMode()
{
}