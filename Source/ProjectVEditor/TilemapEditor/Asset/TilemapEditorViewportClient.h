#pragma once
#include "TilemapEditor/TilemapEditorManager.h"

class UTilemapAsset;
class UBoxComponent;
class UStaticMeshComponent;

class FTilemapEditorViewportClient : public FEditorViewportClient
{
public:
	explicit FTilemapEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene);
	virtual ~FTilemapEditorViewportClient() override;

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

	float HitResultTraceDistance;

	FTilemapEditDelegates::FOnTilemapEditStatueChanged::FDelegate OnTilemapEditStatueChangedHandle;
	FDelegateHandle OnTilemapEditStatueChangedDelegateHandle;

	void OnTilemapEditStatueChanged(bool Statue);

	void GetEditRangeScaleAndLocation(FVector& Location, float& ScaleX, float& ScaleY) const;
};
