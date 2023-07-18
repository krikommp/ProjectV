#pragma once
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DPropertiesTabBody.h"

class UTileSet3DAsset;
class UBoxComponent;
class UStaticMeshComponent;
class UProceduralMeshComponent;
class UInstancedStaticMeshComponent;
class STilemap3DPropertiesTabBody;
class ATilemap3DSelected;

class FTilemap3DEditorViewportClient : public FEditorViewportClient
{
public:
	explicit FTilemap3DEditorViewportClient(TSharedPtr<STilemap3DPropertiesTabBody> InDetailPtr,
	                                        FPreviewScene& InPreviewScene);
	virtual ~FTilemap3DEditorViewportClient() override;

	//~ Begin FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	//~ Begin FEditorViewportClient interface

	void DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize, float ZOffset,
	              const FLinearColor& Color, float Thickness = 3.0f) const;
	void Clear() const;

	int32 GetBlockTextureIndex(const FName& ID, int32 Flag) const;

protected:
	TSharedPtr<STilemap3DPropertiesTabBody> DetailPtr;
	UTilemapAsset* GetTilemapAsset() const { return DetailPtr->GetTilemapAsset(); }
	int32 GetCurrentFloor() const { return DetailPtr->GetCurrentFloor(); }
	const FTileSet3DSubObject& GetCurrentTileProperty() const { return DetailPtr->GetCurrentTileProperty(); }
	UTileSet3DAsset* GetTileSet() const { return DetailPtr->GetTileSet(); }

	TObjectPtr<ULineBatchComponent> LineBatcher;
	TObjectPtr<UBoxComponent> Heightmap;
	TObjectPtr<UStaticMeshComponent> CollisionPlane;
	TObjectPtr<UProceduralMeshComponent> TerrainMesh;
	TObjectPtr<UMaterialInterface> TerrainMat;
	TObjectPtr<UTileSet3DAsset> CurrentTileSet;
	TObjectPtr<ATilemap3DSelected> TilemapSelectedPreview;

	TArray<int32> CachedTilemapSize;

	float HitResultTraceDistance;

	void OnTilemapEditStatueChanged(bool Statue);
	void OnTilemapClearVoxel();
	void OnTilemapGeneratePathFinding();
	void GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const;
};
