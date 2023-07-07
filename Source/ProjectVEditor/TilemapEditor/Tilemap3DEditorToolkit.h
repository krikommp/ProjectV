#pragma once

class STilemap3DEditorViewport;
class STilemap3DPropertiesTabBody;

class FTilemap3DEditorToolkit : public FAssetEditorToolkit, public FGCObject
{
public:
	FTilemap3DEditorToolkit();

	//~ Begin IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	//~ Begin IToolkit interface

	//~ Begin FAssetEditorToolkit interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	//~ Begin FAssetEditorToolkit interface

	//~ Begin FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	//~ Begin FSerializableObject interface
	
	void Initialize(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UTilemapAsset* Asset);
	
public:
	UTilemapAsset* GetTilemapBeingEdited() const { return TilemapBeingEdited; }
	TSharedPtr<STilemap3DPropertiesTabBody> GetDetailPtr() const { return DetailPtr; }
	
	TObjectPtr<UTilemapAsset> TilemapBeingEdited;

	TSharedPtr<STilemap3DEditorViewport> ViewportPtr;

	TSharedPtr<STilemap3DPropertiesTabBody> DetailPtr;

protected:
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

protected:
	static const FName ViewportID;
	static const FName DetailsID;
};
