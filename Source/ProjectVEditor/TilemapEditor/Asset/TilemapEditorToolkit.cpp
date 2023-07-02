#include "TilemapEditorToolkit.h"

#include "KismetWidgets/Public/SSingleObjectDetailsPanel.h"
#include "Tilemap/TilemapAsset.h"
#include "TilemapEditor/TilemapEditorViewport.h"
#define LOCTEXT_NAMESPACE "FTilemapEditorToolkit"

const FName FTilemapEditorToolkit::ViewportID(TEXT("TilemapViewport"));
const FName FTilemapEditorToolkit::DetailsID(TEXT("TilemapDetails"));

FTilemapEditorToolkit::FTilemapEditorToolkit()
{
}

void FTilemapEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FTilemapEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(DetailsID);
	InTabManager->UnregisterTabSpawner(ViewportID);
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
				                             FTabManager::NewSplitter()
				                             ->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				                             ->Split
				                             (
					                             FTabManager::NewStack()
					                             ->SetSizeCoefficient(0.7f)
					                             ->AddTab(ViewportID, ETabState::OpenedTab)
				                             )
			                             )
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	const FName AppIdentifier = TEXT("TilemapAssetEditor");
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, Layout, bCreateDefaultStandaloneMenu,
	                                     bCreateDefaultToolbar, Asset);
}

TSharedRef<SDockTab> FTilemapEditorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			SNew(STilemapEditorViewport)
		];
}

TSharedRef<SDockTab> FTilemapEditorToolkit::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	TSharedRef<FTilemapEditorToolkit> EditorToolkit = SharedThis(this);

	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTab_Tile", "Details"))
		[
			SNew(SSingleObjectDetailsPanel, EditorToolkit)
		];
}

#undef LOCTEXT_NAMESPACE
