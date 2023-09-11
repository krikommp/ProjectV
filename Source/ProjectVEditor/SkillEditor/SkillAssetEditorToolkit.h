#pragma once

class SSkillEditorLayout;

class FSkillAssetEditorToolkit final : public FAssetEditorToolkit, public FGCObject
{
public:
	FSkillAssetEditorToolkit();

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
	
	void Initialize(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class USkillBaseAsset* Asset);

protected:
	TSharedRef<SDockTab> SpawnTab_Edit(const FSpawnTabArgs& Args);
	
	static const FName EditTabID;

public:
	TObjectPtr<USkillBaseAsset> SkillBaseAsset;
	TSharedPtr<SSkillEditorLayout> SkillEditWidget;
};
