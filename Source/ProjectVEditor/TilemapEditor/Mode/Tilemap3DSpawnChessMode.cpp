#include "Tilemap3DSpawnChessMode.h"

#include "GridTraceChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Preview/Tilemap3DPreviewChess.h"

FTilemap3DSpawnChessMode::FTilemap3DSpawnChessMode()
	: HitResultTraceDistance(10000.0), BlockIndex(INDEX_NONE)
{
	EditMode = EEM_Chess_Spawn;
}

void FTilemap3DSpawnChessMode::EnterMode()
{
}

void FTilemap3DSpawnChessMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
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
			FVector HitLocation = HitResult.Location;
			HitLocation.Z -= ViewportClient->GetTilemapAsset()->GridSize * 0.5f;
			const int32 Index = ViewportClient->GetTilemapAsset()->VectorToIndex(HitLocation);

			if (ViewportClient->GetTilemapAsset()->Blocks[Index].MeshIndex != FName())
				return;

			if (ViewportClient->GetTilemapAsset()->Blocks[Index].ChessID != FName())
				return;

			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = Index / (ViewportClient->GetTilemapAsset()->LevelSizeX * ViewportClient->GetTilemapAsset()->
				LevelSizeY) * ViewportClient->GetTilemapAsset()->GridSize + ViewportClient->GetTilemapAsset()->GridSize;
			Transform.SetLocation(Location);

			FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[Index];

			auto ChessPiece = ViewportClient->GetWorld()->SpawnActor<ATilemap3DPreviewChess>();
			ChessPiece->SetupSkeletalMeshAsset(ViewportClient->GetTileChess().SkeletalMesh);
			ChessPiece->SetActorTransform(Transform);

			Block.ChessID = ViewportClient->GetTileChess().HeroID;
			Block.ChessTransform = Transform;
			Block.ChessInEditor = ChessPiece;
			BlockIndex = Index;
		}
	}
	if (EventArgs.Key == EKeys::R && EventArgs.Event == IE_Pressed && BlockIndex != INDEX_NONE)
	{
		FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[BlockIndex];
		if (Block.ChessInEditor != nullptr)
		{
			FTransform Transform;
			Transform = Block.ChessInEditor->GetActorTransform();
			FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f));
			Transform.SetRotation(Rotation * Transform.GetRotation());
			Block.ChessInEditor->SetActorTransform(Transform);
			Block.ChessTransform = Transform;
		}
	}
}

void FTilemap3DSpawnChessMode::ExitMode()
{
	BlockIndex = INDEX_NONE;
}
