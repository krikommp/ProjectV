#pragma once
#include "TilemapEditor/TilemapEditorManager.h"

class UTilemapAsset;
class UBoxComponent;

class FTilemapEditorViewportClient : public FEditorViewportClient
{
public:
	explicit FTilemapEditorViewportClient(UTilemapAsset* InAsset, FPreviewScene& InPreviewScene);
	virtual ~FTilemapEditorViewportClient() override;

	//~ Begin FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ Begin FEditorViewportClient interface

	void DrawGrid(const FVector& Location, int32 RowCount, int32 ColCount, float CellSize, float ZOffset, const FLinearColor& Color, float Thickness = 3.0f) const;
	void Clear() const;

protected:
	TObjectPtr<UTilemapAsset> TilemapBeingEdited;
	TObjectPtr<ULineBatchComponent> LineBatcher;
	TObjectPtr<UBoxComponent> Heightmap;

	FTilemapEditDelegates::FOnTilemapEditStatueChanged::FDelegate OnTilemapEditStatueChangedHandle;
	FDelegateHandle OnTilemapEditStatueChangedDelegateHandle;

	void OnTilemapEditStatueChanged(bool Statue);
};
