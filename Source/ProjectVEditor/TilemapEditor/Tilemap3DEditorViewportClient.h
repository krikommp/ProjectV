#pragma once
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DPropertiesTabBody.h"

class FTilemap3DBaseMode;
class UTileSet3DAsset;
class UBoxComponent;
class UStaticMeshComponent;
class UProceduralMeshComponent;
class UInstancedStaticMeshComponent;
class STilemap3DPropertiesTabBody;
class ATilemap3DSelected;
class UTextRenderComponent;

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

	void AddMesh(const FName& ID, UStaticMesh* Mesh, const FTransform& Transform, int32 Index);

	int32 GetBlockTextureIndex(const FName& ID, int32 Flag) const;
	TSharedPtr<STilemap3DPropertiesTabBody> DetailPtr;
	UTilemapAsset* GetTilemapAsset() const { return DetailPtr->GetTilemapAsset(); }
	int32 GetCurrentFloor() const { return DetailPtr->GetCurrentFloor(); }
	ETilemap3DEditMode GetEditMode() const { return DetailPtr->GetEditMode(); }
	const FTileSet3DMesh& GetTileMesh() const { return DetailPtr->GetTileMesh(); }
	const FTileSet3DCube& GetTileCube() const { return DetailPtr->GetTileCube(); }
	UTileSet3DAsset* GetTileSet() const { return DetailPtr->GetTileSet(); }
	UProceduralMeshComponent* GetTerrainMesh() const { return TerrainMesh; }
	UMaterialInterface* GetTerrainMat() const { return TerrainMat; }
protected:

	TObjectPtr<ULineBatchComponent> LineBatcher;
	TObjectPtr<UBoxComponent> Heightmap;
	TObjectPtr<UStaticMeshComponent> CollisionPlane;
	TObjectPtr<UProceduralMeshComponent> TerrainMesh;
	TObjectPtr<UMaterialInterface> TerrainMat;
	TObjectPtr<UTileSet3DAsset> CurrentTileSet;
	TObjectPtr<ATilemap3DSelected> TilemapSelectedPreview;
	TArray<TObjectPtr<UTextRenderComponent>> PathFindingIndexTextArray;
	TArray<TObjectPtr<UTextRenderComponent>> PathFindingEdgeCostTextArray;
	TMap<FName, TObjectPtr<UInstancedStaticMeshComponent>> MeshSet;

	TArray<TSharedPtr<FTilemap3DBaseMode>> EditModes;

	TArray<int32> CachedTilemapSize;

	float HitResultTraceDistance;

	void OnTilemapEditStatueChanged(bool Statue);
	void OnTilemapClearVoxel();
	void OnTilemapGeneratePathFinding();
	void DisplayPathFinding();
	void GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const;
};
