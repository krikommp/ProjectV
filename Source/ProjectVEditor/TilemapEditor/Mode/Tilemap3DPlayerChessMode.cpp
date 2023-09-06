#include "Tilemap3DPlayerChessMode.h"

#include "GridTraceChannel.h"
#include "Components/DecalComponent.h"
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

			if (ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->ChessData != nullptr)
				return;

			if (ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->PlayerStartDecal != nullptr)
				return;

			FTransform Transform = FTransform::Identity;
			FVector Location = ViewportClient.Pin()->GetTilemapAsset()->IndexToVector(Index);
			Location.Z = Index / (ViewportClient.Pin()->GetTilemapAsset()->LevelSizeX * ViewportClient.Pin()->GetTilemapAsset()->
				LevelSizeY) * ViewportClient.Pin()->GetTilemapAsset()->GridSize + ViewportClient.Pin()->GetTilemapAsset()->GridSize;
			Location.Z += 1.0f;
			Transform.SetLocation(Location);
			Transform.SetRotation(FQuat({90.0f, 0.0f, 0.0f}));

			UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();

			const FVector DecalSize = { 95.0f, 45.0f, 45.0f };
			UDecalComponent* DecalComponent = NewObject<UDecalComponent>();
			ViewportClient.Pin()->GetPreviewScene()->AddComponent(DecalComponent, Transform);
			DecalComponent->bAllowAnyoneToDestroyMe = true;
			DecalComponent->SetDecalMaterial(Settings->DefaultPlayerChessRangeMat.LoadSynchronous());
			DecalComponent->DecalSize = DecalSize;
			DecalComponent->SetUsingAbsoluteScale(true);

			ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->PlayerStartDecal = DecalComponent;
			ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->bPlayerChessStart = true;
		}
	}

	if (EventArgs.Key == EKeys::RightMouseButton && EventArgs.Event == IE_Pressed)
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

			if (ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->PlayerStartDecal == nullptr)
				return;

			ViewportClient.Pin()->GetPreviewScene()->RemoveComponent(ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->PlayerStartDecal);
			ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->PlayerStartDecal->DestroyComponent();
			ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->PlayerStartDecal = nullptr;
			ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index]->bPlayerChessStart = false;
		}
	}
}

void FTilemap3DPlayerChessMode::ExitMode()
{
}