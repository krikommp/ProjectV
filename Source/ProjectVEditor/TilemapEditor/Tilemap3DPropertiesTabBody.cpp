#include "Tilemap3DPropertiesTabBody.h"

#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorManager.h"
#include "Tilemap3DEditorSettings.h"
#include "Engine/Texture2DArray.h"
#include "Widget/Tilemap3DEditModeWidget.h"
#include "Widget/Tilemap3DEditStatusWidget.h"
#include "Widget/Tilemap3DFloorStatusWidget.h"
#include "Widget/TileSetGalleyWidget.h"

#define LOCTEXT_NAMESPACE "STilemap3DPropertiesTabBody"

void STilemap3DPropertiesTabBody::Construct(const FArguments& InArgs,
                                            TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor,
                                            TObjectPtr<UTileSet3DAsset> InTileSet)
{
	TilemapEditorPtr = InTilemapEditor;
	TileSet = InTileSet;
	bEditProperty = false;
	CurrentFloor = 0;

	const UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();
	CurrentTileCube = Settings->DefaultTileSet.LoadSynchronous()->TileCubeSets[0];

	SSingleObjectDetailsPanel::Construct(
		SSingleObjectDetailsPanel::FArguments().HostCommandList(InTilemapEditor->GetToolkitCommands()).
		                                        HostTabManager(InTilemapEditor->GetTabManager()),
		/*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/ true);
}

UObject* STilemap3DPropertiesTabBody::GetObjectToObserve() const
{
	return TilemapEditorPtr.Pin()->GetTilemapBeingEdited();
}

TSharedRef<SWidget> STilemap3DPropertiesTabBody::PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget)
{
	PropertyEditorWidget->SetEnabled(TAttribute<bool>::Create([this]()-> bool
	{
		return !bEditProperty;
	}));

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			PropertyEditorWidget
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			[
				SNew(STilemap3DEditStatusWidget, bEditProperty)
				.OnEditStatusChanged_Lambda([this](bool Status)
				{
					bEditProperty = Status;
					CurrentFloor = FMath::Min(TilemapEditorPtr.Pin()->TilemapBeingEdited->Floors - 1,
					                          CurrentFloor);
					if (!bEditProperty) CurrentEditMode = EEM_View;
					FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(bEditProperty);
				})
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
		.Visibility_Lambda([this]()
			             {
				             return bEditProperty ? EVisibility::Visible : EVisibility::Collapsed;
			             })
			[
				SNew(STilemap3DEditModeWidget)
				.OnEditModeChanged_Lambda([this](const ETilemap3DEditMode InEditMode)
				{
					FTilemap3DEditDelegates::OnTilemapEditModeChanged.Broadcast(CurrentEditMode, InEditMode);
					CurrentEditMode = InEditMode;
					//todo..
				})
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return bEditProperty ? EVisibility::Visible : EVisibility::Collapsed;
			             })
			[
				SNew(STilemap3DFloorStatusWidget)
				.Floor_Lambda([this]()
				                                 {
					                                 return CurrentFloor;
				                                 })
				.OnFloorIncrease_Lambda([this](const int32 Floor)
				                                 {
					                                 CurrentFloor = FMath::Min(
						                                 TilemapEditorPtr.Pin()->TilemapBeingEdited->Floors - 1,
						                                 CurrentFloor + 1);
					                                 FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(
						                                 bEditProperty);
				                                 })
				.OnFloorDecrease_Lambda([this](const int32 Floor)
				                                 {
					                                 CurrentFloor = FMath::Max(CurrentFloor - 1, 0);
					                                 FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(
						                                 bEditProperty);
				                                 })
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			[
				SNew(STileSetGalleyWidget)
				.TileSet_Lambda([this]() { return TileSet; })
				.EditMode_Lambda([this]() { return CurrentEditMode; })
				.OnClicked_Lambda([this](const FName& ID)
				                          {
					                          if (const auto* TileCube = TileSet->TileCubeSets.FindByPredicate(
						                          [=](const auto& Item)
						                          {
							                          return Item.ID == ID;
						                          }))
					                          {
						                          CurrentTileCube = *TileCube;
					                          }
					                          else if (const auto* TileMesh = TileSet->TileMeshSets.FindByPredicate(
						                          [=](const auto& Item)
						                          {
							                          return Item.ID == ID;
						                          }))
					                          {
						                          CurrentTileMesh = *TileMesh;
					                          }
				                          })
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			[
				SNew(SButton)
				.OnClicked_Lambda([this]()
				{
					FTilemap3DEditDelegates::OnTilemapGeneratePathFinding.Broadcast();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(LOCTEXT("TilemapPathFindingGenerate", "Generate path finding data."))
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return bEditProperty ? EVisibility::Visible : EVisibility::Collapsed;
			             })
			[
				SNew(SButton)
				.OnClicked_Lambda([this]()
				{
					FTilemap3DEditDelegates::OnTilemapClearVoxel.Broadcast();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
						.Justification(ETextJustify::Center)
						.Text(LOCTEXT("TilemapFloorResetTileset", "Reset Tileset"))
				]
			]
		];
}

UTileSet3DAsset* STilemap3DPropertiesTabBody::GetTileSet() const
{
	if (TileSet.IsValid())
		return TileSet.Get();
	return nullptr;
}

int32 STilemap3DPropertiesTabBody::GetTextureIndex(UTexture2D* InTexture) const
{
	for (int32 SourceIndex = 0; SourceIndex < TileSet->Textures->SourceTextures.Num(); ++SourceIndex)
	{
		if (TileSet->Textures->SourceTextures[SourceIndex] == InTexture)
		{
			return SourceIndex;
		}
	}
	return -1;
}

#undef LOCTEXT_NAMESPACE
