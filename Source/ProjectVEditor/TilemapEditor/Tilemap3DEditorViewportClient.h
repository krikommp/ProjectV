#pragma once
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorManager.h"

class UTilemapAsset;
class UBoxComponent;
class UStaticMeshComponent;
class UProceduralMeshComponent;

class FTilemap3DEditorViewportClient : public FEditorViewportClient
{
public:
	explicit FTilemap3DEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene);
	virtual ~FTilemap3DEditorViewportClient() override;

	//~ Begin FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	//~ Begin FEditorViewportClient interface

	void DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize, float ZOffset,
	              const FLinearColor& Color, float Thickness = 3.0f) const;
	void Clear() const;

protected:
	TObjectPtr<UTilemapAsset> TilemapBeingEdited;
	TObjectPtr<ULineBatchComponent> LineBatcher;
	TObjectPtr<UBoxComponent> Heightmap;
	TObjectPtr<UStaticMeshComponent> CollisionPlane;
	TObjectPtr<UProceduralMeshComponent> TerrainMesh;

	float HitResultTraceDistance;

	FTilemap3DEditDelegates::FOnTilemapEditStatueChanged::FDelegate OnTilemapEditStatueChangedHandle;
	FDelegateHandle OnTilemapEditStatueChangedDelegateHandle;

	TArray<EBlock> Blocks;

	void OnTilemapEditStatueChanged(bool Statue);

	void GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const;

	int32 VectorToIndex(const FVector& Location, int32 Floor) const;
};
