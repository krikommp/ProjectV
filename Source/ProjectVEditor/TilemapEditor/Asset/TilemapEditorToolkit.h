#pragma once

class FTilemapEditorToolkit : public FAssetEditorToolkit, public FGCObject
{
public:
	FTilemapEditorToolkit();

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

	TSharedRef<SDockTab> SpawnTestTabs(const FSpawnTabArgs& Args);

protected:
	static const FName TestTabId;
};
