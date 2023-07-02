#include "TilemapEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"
#define LOCTEXT_NAMESPACE "FTilemapEditorToolkit"

const FName FTilemapEditorToolkit::TestTabId(TEXT("TilemapAssetEditor_TestTab"));

FTilemapEditorToolkit::FTilemapEditorToolkit()
{
}

void FTilemapEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(TestTabId, FOnSpawnTab::CreateSP(this, &FTilemapEditorToolkit::SpawnTestTabs)).
	            SetDisplayName(LOCTEXT("TilemapAssetTestTab", "TilemapAssetTestTab"));
}

void FTilemapEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(TestTabId);
}

FName FTilemapEditorToolkit::GetToolkitFName() const
{
	return FName("TilemapAssetName");
}

FText FTilemapEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("TilemapAssetLabel", "TilemapAssetBase");
}

FString FTilemapEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("TilemapAssetTabPrefix", "TilemapAssetPrefix ").ToString();
}

FLinearColor FTilemapEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0, 0.5f, 0.6f, 0.5f);
}

void FTilemapEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	//Collector.AddReferencedObject()
}

FString FTilemapEditorToolkit::GetReferencerName() const
{
	return TEXT("FTilemapEditorToolkit");
}

void FTilemapEditorToolkit::Initialize(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
                                       UTilemapAsset* Asset)
{
	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("TilemapAssetEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(TestTabId, ETabState::OpenedTab)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	const FName AppIdentifier = TEXT("TilemapAssetEditor");
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, Layout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Asset);
}

TSharedRef<SDockTab> FTilemapEditorToolkit::SpawnTestTabs(const FSpawnTabArgs& Args)
{
	return
		SNew(SDockTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString("Test"))
			]
		];
}

#undef LOCTEXT_NAMESPACE
