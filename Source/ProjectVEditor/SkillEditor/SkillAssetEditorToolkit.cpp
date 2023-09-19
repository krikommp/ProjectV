#include "SkillAssetEditorToolkit.h"

#include "EditorInterface/Layout/SkillEditorLayout.h"
#include "..\..\ProjectVGame\Skill\SkillAsset.h"

#define LOCTEXT_NAMESPACE "FSkillAssetEditorToolkit"

const FName FSkillAssetEditorToolkit::EditTabID(TEXT("SkillEditTab"));

FSkillAssetEditorToolkit::FSkillAssetEditorToolkit()
{
}

void FSkillAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
		LOCTEXT("WorkSpaceMenu_SkillEd", "Skill Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FSkillAssetEditorToolkit::EditTabID,
	                                 FOnSpawnTab::CreateSP(this, &FSkillAssetEditorToolkit::SpawnTab_Edit))
	            .SetDisplayName(LOCTEXT("SkillEditTabLabel", "SkillEditTab"))
	            .SetGroup(WorkspaceMenuCategoryRef)
	            .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FSkillAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(EditTabID);
}

FName FSkillAssetEditorToolkit::GetToolkitFName() const
{
	return FName("SkillAssetName");
}

FText FSkillAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("SkillAssetLabel", "SkillAssetBase");
}

FString FSkillAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("SkillAssetTabPrefix", "SkillAssetPrefix ").ToString();
}

FLinearColor FSkillAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0, 0.5f, 0.6f, 0.5f);
}

void FSkillAssetEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(SkillBaseAsset);
}

FString FSkillAssetEditorToolkit::GetReferencerName() const
{
	return TEXT("FSkillAssetEditorToolkit");
}

void FSkillAssetEditorToolkit::Initialize(const EToolkitMode::Type Mode,
                                          const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* Asset)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseOtherEditors(Asset, this);
	SkillBaseAsset = Asset;

	SkillEditWidget = SNew(SSkillEditorLayout, SkillBaseAsset);

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("SkillAssetEditor_Layout")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.9f)
				->SetHideTabWell(false)
				->AddTab(FSkillAssetEditorToolkit::EditTabID, ETabState::OpenedTab))
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	const FName AppIdentifier = TEXT("SkillAssetEditor");
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, Layout, bCreateDefaultStandaloneMenu,
	                                     bCreateDefaultToolbar, Asset);
}

TSharedRef<SDockTab> FSkillAssetEditorToolkit::SpawnTab_Edit(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(LOCTEXT("SkillBaseInfoTab_Tile", "BaseInfo"))
		[
			SkillEditWidget.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE
