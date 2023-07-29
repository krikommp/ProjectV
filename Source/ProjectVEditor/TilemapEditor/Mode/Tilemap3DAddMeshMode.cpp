#include "Tilemap3DAddMeshMode.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DAddMeshMode::FTilemap3DAddMeshMode(const TSharedPtr<FTilemap3DEditorViewportClient>& InViewportClient)
	: FTilemap3DBaseMode(InViewportClient), HitResultTraceDistance(10000.0), InstancedIndex(INDEX_NONE), BlockIndex(INDEX_NONE)
{
}

void FTilemap3DAddMeshMode::EnterMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}

void FTilemap3DAddMeshMode::InputKey(const FInputKeyEventArgs& EventArgs)
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
			if (ViewportClient.Pin()->GetTileMesh() == nullptr)
				return;
			
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

			UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[Index];
			Block->MeshIndex = ViewportClient.Pin()->GetTileMesh()->ID;
			Block->MeshTransform = Transform;

			Block->MeshInstancedIndex = FTilemap3DTileMeshGenerator::AddTileMesh({
				ViewportClient.Pin()->GetTilemapAsset(),
				ViewportClient.Pin()->GetPreviewScene(),
				ViewportClient.Pin()->GetTileMeshMap(),
				Block->MeshIndex,
				ViewportClient.Pin()->GetTileMesh()->Mesh,
				Transform,
				Index,
				ViewportClient.Pin()->GetTileMesh()->Material
			});

			InstancedIndex =Block->MeshInstancedIndex;
			BlockIndex = Index;
			MeshComponent = ViewportClient.Pin()->GetTileMeshMap()[Block->MeshIndex];
		}
	}
	if (EventArgs.Key == EKeys::R && EventArgs.Event == IE_Pressed)
	{
		if (MeshComponent != nullptr && InstancedIndex != INDEX_NONE && ViewportClient.Pin()->GetTilemapAsset()->Blocks.IsValidIndex(BlockIndex))
		{
			FTransform Transform;
			MeshComponent->GetInstanceTransform(InstancedIndex, Transform);
			FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f));
			Transform.SetRotation(Rotation * Transform.GetRotation());
			MeshComponent->UpdateInstanceTransform(InstancedIndex, Transform, false, true, false);

			UBlock* Block = ViewportClient.Pin()->GetTilemapAsset()->Blocks[BlockIndex];
			Block->MeshTransform = Transform;
		}
	}
}

void FTilemap3DAddMeshMode::ExitMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}
