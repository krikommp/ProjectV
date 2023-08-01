#include "Tilemap3DRemoveChessMode.h"

#include "ProjectVEditor.h"
#include "Chess/GridChessData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DRemoveChessMode::FTilemap3DRemoveChessMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	:FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0f)
{
}

void FTilemap3DRemoveChessMode::EnterMode()
{
}

void FTilemap3DRemoveChessMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			UEngineTypes::ConvertToTraceType(TilemapChessTrace),
			false,
			IgnoreActor,
			EDrawDebugTrace::None,
			HitResult,
			false);
		if (HitResult.bBlockingHit)
		{
			const int32 BottomIndex = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitResult.Location, 0);
			UBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient.Pin()->GetTilemapAsset()->Floors; ++z)
			{
				const int32 Index = BottomIndex + z * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeY;
				if (!ViewportClient.Pin()->GetTilemapAsset()->Blocks.IsValidIndex(Index))
					continue;
				UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index];
				if (Block->ChessData != nullptr)
				{
					if (Block->ChessData->ChessTransform.GetLocation().Z < HitResult.Location.Z)
					{
						BottomBlock = Block;
					}
				}
			}

			if (BottomBlock != nullptr && BottomBlock->ChessData != nullptr)
			{
				BottomBlock->ChessInEditor->Destroy();
				BottomBlock->ChessInEditor = nullptr;
				BottomBlock->ChessData = nullptr;
			}
		}
	}
}

void FTilemap3DRemoveChessMode::ExitMode()
{
}
