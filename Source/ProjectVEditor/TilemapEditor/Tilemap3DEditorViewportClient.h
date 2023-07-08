#pragma once
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorManager.h"
#include "Tilemap3DPropertiesTabBody.h"

class UBoxComponent;
class UStaticMeshComponent;
class UProceduralMeshComponent;
class UInstancedStaticMeshComponent;
class STilemap3DPropertiesTabBody;
class ATilemap3DSelected;

class FTilemap3DEditorViewportClient : public FEditorViewportClient
{
public:
	explicit FTilemap3DEditorViewportClient(TSharedPtr<STilemap3DPropertiesTabBody> InDetailPtr, FPreviewScene& InPreviewScene);
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
	TSharedPtr<STilemap3DPropertiesTabBody> DetailPtr;
	UTilemapAsset* GetTilemapAsset() const { return DetailPtr->GetTilemapAsset(); }
	int32 GetCurrentFloor() const { return DetailPtr->GetCurrentFloor(); }
	
	TObjectPtr<ULineBatchComponent> LineBatcher;
	TObjectPtr<UBoxComponent> Heightmap;
	TObjectPtr<UStaticMeshComponent> CollisionPlane;
	TObjectPtr<UProceduralMeshComponent> TerrainMesh;
	TObjectPtr<UInstancedStaticMeshComponent> TerrainInstancedMesh;
	TObjectPtr<ATilemap3DSelected> TilemapSelectedPreview;

	float HitResultTraceDistance;

	void OnTilemapEditStatueChanged(bool Statue);
	void OnTilemapModelChanged();

	void GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const;

	int32 VectorToIndex(const FVector& Location, int32 Floor) const;
};
