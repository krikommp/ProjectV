#include "Tilemap3DAddMeshMode.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ProjectVEditor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TilemapEditor/Tilemap3DEditorViewportClient.h"
#include "TilemapEditor/Generator/Tilemap3DTileMeshGenerator.h"

FTilemap3DAddMeshMode::FTilemap3DAddMeshMode()
	: HitResultTraceDistance(10000.0), InstancedIndex(INDEX_NONE), BlockIndex(INDEX_NONE)
{
	EditMode = EEM_Mesh_Append;
}

void FTilemap3DAddMeshMode::EnterMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}

void FTilemap3DAddMeshMode::InputKey(FTilemap3DEditorViewportClient* ViewportClient,
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

			InstancedIndex = Block.MeshInstancedIndex;
			BlockIndex = Index;
			MeshComponent = ViewportClient->GetTileMeshMap()[Block.MeshIndex];
		}
	}
	if (EventArgs.Key == EKeys::R && EventArgs.Event == IE_Pressed)
	{
		if (MeshComponent != nullptr && InstancedIndex != INDEX_NONE && ViewportClient->GetTilemapAsset()->Blocks.IsValidIndex(BlockIndex))
		{
			FTransform Transform;
			MeshComponent->GetInstanceTransform(InstancedIndex, Transform);
			FQuat Rotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f));
			Transform.SetRotation(Rotation * Transform.GetRotation());
			MeshComponent->UpdateInstanceTransform(InstancedIndex, Transform, false, true, false);

			FBlock& Block = ViewportClient->GetTilemapAsset()->Blocks[BlockIndex];
			Block.MeshTransform = Transform;
		}
	}
}

void FTilemap3DAddMeshMode::ExitMode()
{
	InstancedIndex = INDEX_NONE;
	BlockIndex = INDEX_NONE;
	MeshComponent = nullptr;
}
