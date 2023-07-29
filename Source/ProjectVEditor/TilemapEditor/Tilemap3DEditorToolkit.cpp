#include "Tilemap3DEditorToolkit.h"

#include "Tilemap3DEditorSettings.h"
#include "Tilemap3DPropertiesTabBody.h"
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorViewport.h"

#define LOCTEXT_NAMESPACE "FTilemap3DEditorToolkit"

const FName FTilemap3DEditorToolkit::ViewportID(TEXT("TilemapViewport"));
const FName FTilemap3DEditorToolkit::DetailsID(TEXT("TilemapDetails"));

FTilemap3DEditorToolkit::FTilemap3DEditorToolkit()
{
}

void FTilemap3DEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
		LOCTEXT("WorkSpaceMenu_TilemapEditor", "Tilemap Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FTilemap3DEditorToolkit::ViewportID,
	                                 FOnSpawnTab::CreateSP(this, &FTilemap3DEditorToolkit::SpawnTab_Viewport))
	            .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FTilemap3DEditorToolkit::DetailsID,
	                                 FOnSpawnTab::CreateSP(this, &FTilemap3DEditorToolkit::SpawnTab_Details))
	            .SetDisplayName(LOCTEXT("DetailsTabLabel", "Details"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FTilemap3DEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(DetailsID);
	InTabManager->UnregisterTabSpawner(ViewportID);
}

FName FTilemap3DEditorToolkit::GetToolkitFName() const
{
	return FName("TilemapAssetName");
}

FText FTilemap3DEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("TilemapAssetLabel", "TilemapAssetBase");
}

FString FTilemap3DEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("TilemapAssetTabPrefix", "TilemapAssetPrefix ").ToString();
}

FLinearColor FTilemap3DEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0, 0.5f, 0.6f, 0.5f);
}

void FTilemap3DEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(TilemapBeingEdited);
}

FString FTilemap3DEditorToolkit::GetReferencerName() const
{
	return TEXT("FTilemap3DEditorToolkit");
}

void FTilemap3DEditorToolkit::Initialize(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost,
                                       UTilemapAsset* Asset)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseOtherEditors(Asset, this);
	TilemapBeingEdited = Asset;

	const UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();
	TSharedRef<FTilemap3DEditorToolkit> EditorToolkit = SharedThis(this);
	DetailPtr = SNew(STilemap3DPropertiesTabBody, EditorToolkit, TilemapBeingEdited->TileSet == nullptr ? Settings->DefaultTileSet.LoadSynchronous() : TilemapBeingEdited->TileSet);

	ViewportPtr = SNew(STilemap3DEditorViewport)
		.TilemapDetailPtr(this, &FTilemap3DEditorToolkit::GetDetailPtr);

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("TilemapAssetEditor_Layout")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(FTilemap3DEditorToolkit::ViewportID, ETabState::OpenedTab)
				)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab(FTilemap3DEditorToolkit::DetailsID, ETabState::OpenedTab)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	const FName AppIdentifier = TEXT("TilemapAssetEditor");
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, Layout, bCreateDefaultStandaloneMenu,
	                                     bCreateDefaultToolbar, Asset);
}

TSharedRef<SDockTab> FTilemap3DEditorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			ViewportPtr.ToSharedRef()
		];
}

TSharedRef<SDockTab> FTilemap3DEditorToolkit::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTab_Tile", "Details"))
		[
			DetailPtr.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE
