#include "Tilemap3DSelectChessMode.h"

#include "ProjectVEditor.h"
#include "ChessPieces/GridChessPieceData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"

FTilemap3DSelectChessMode::FTilemap3DSelectChessMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0), BlockIndex(INDEX_NONE)
{
}

void FTilemap3DSelectChessMode::EnterMode()
{
	ViewportClient.Pin()->SetChessData(nullptr);
	BlockIndex = INDEX_NONE;
}

void FTilemap3DSelectChessMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			const UBlock* BottomBlock = nullptr;

			for (int32 z = 0; z < ViewportClient.Pin()->GetTilemapAsset()->Floors; ++z)
			{
				const int32 Index = BottomIndex + z * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->LevelSizeY;
				if (!ViewportClient.Pin()->GetTilemapAsset()->Blocks.IsValidIndex(Index))
					continue;
				const UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index];
				if (Block->ChessData != nullptr)
				{
					if (Block->ChessData->ChessTransform.GetLocation().Z < HitResult.Location.Z)
					{
						BottomBlock = Block;
						BlockIndex = Index;
					}
				}
			}

			if (BottomBlock != nullptr && BottomBlock->ChessData != nullptr)
			{
				ViewportClient.Pin()->SetChessData(BottomBlock->ChessData);
			}
		}
	}
	if (EventArgs.Key == EKeys::R && EventArgs.Event == IE_Pressed && BlockIndex != INDEX_NONE)
	{
		UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[BlockIndex];
		if (Block->ChessInEditor != nullptr &&Block->ChessData != nullptr)
		{
			FTransform Transform;
			Transform =Block->ChessInEditor->GetActorTransform();
			FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f));
			Transform.SetRotation(Rotation * Transform.GetRotation());
			Block->ChessInEditor->SetActorTransform(Transform);
			Block->ChessData->ChessTransform = Transform;
		}
	}
}

void FTilemap3DSelectChessMode::ExitMode()
{
	ViewportClient.Pin()->SetChessData(nullptr);
	BlockIndex = INDEX_NONE;
}
