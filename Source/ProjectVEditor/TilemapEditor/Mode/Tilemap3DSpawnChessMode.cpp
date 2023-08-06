#include "Tilemap3DSpawnChessMode.h"

#include "GridTraceChannel.h"
#include "ProjectVEditor.h"
#include "Chess/GridChessData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Preview/Tilemap3DPreviewChess.h"

FTilemap3DSpawnChessMode::FTilemap3DSpawnChessMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0), BlockIndex(INDEX_NONE)
{
}

void FTilemap3DSpawnChessMode::EnterMode()
{
}

void FTilemap3DSpawnChessMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			if (ViewportClient.Pin()->GetTileChess() == nullptr)
				return;
			
			FVector HitLocation = HitResult.Location;
			HitLocation.Z -= ViewportClient.Pin()->GetTilemapAsset()->GridSize * 0.5f;
			const int32 Index = ViewportClient.Pin()->GetTilemapAsset()->VectorToIndex(HitLocation);

			if (ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->MeshIndex != FName())
				return;

			if (ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->ChessData != nullptr)
				return;

			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient.Pin()->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = Index / (ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->
				LevelSizeY) * ViewportClient.Pin()->GetTilemapAsset()->GridSize + ViewportClient.Pin()->GetTilemapAsset()->GridSize;
			Transform.SetLocation(Location);

			UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index];

			auto ChessPiece = ViewportClient.Pin()->GetWorld()->SpawnActor<ATilemap3DPreviewChess>();
			ChessPiece->SetupSkeletalMeshAsset(ViewportClient.Pin()->GetTileChess()->SkeletalMesh);
			ChessPiece->SetCollisionResponseToChannel(TilemapChessTrace, ECR_Block);
			ChessPiece->SetActorTransform(Transform);

			Block->ChessData = NewObject<UGridChessData>(Block);
			Block->ChessData->ChessID = ViewportClient.Pin()->GetTileChess()->HeroID;
			Block->ChessData->ChessTransform = Transform;
			Block->ChessInEditor = ChessPiece;
			BlockIndex = Index;
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

void FTilemap3DSpawnChessMode::ExitMode()
{
	BlockIndex = INDEX_NONE;
}
