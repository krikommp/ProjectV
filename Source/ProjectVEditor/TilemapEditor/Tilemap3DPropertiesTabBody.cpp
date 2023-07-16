#include "Tilemap3DPropertiesTabBody.h"

#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorManager.h"
#include "Tilemap3DEditorSettings.h"
#include "Engine/Texture2DArray.h"
#include "Widget/TileSetGalleyWidget.h"

#define LOCTEXT_NAMESPACE "STilemap3DPropertiesTabBody"

TSharedRef<SWidget> STilemap3DPropertiesTabBody::DrawEditStatusWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .FillWidth(1.0f)
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Fill)
		  .Padding(2.0f)
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				bEditProperty = !bEditProperty;
				this->EditStatusText->SetText(bEditProperty
					                              ? LOCTEXT("Stop", "Stop Editing")
					                              : LOCTEXT("Start", "Start Editing"));
				CurrentFloor = FMath::Min(TilemapEditorPtr.Pin()->TilemapBeingEdited->Floors - 1, CurrentFloor.Get());
				FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(bEditProperty);
				return FReply::Handled();
			})
			[
				SAssignNew(EditStatusText, STextBlock)
						.Justification(ETextJustify::Center)
						.Text(bEditProperty
							      ? LOCTEXT("Stop", "Stop Editing")
							      : LOCTEXT("Start", "Start Editing"))
			]
		];
}

void STilemap3DPropertiesTabBody::Construct(const FArguments& InArgs,
                                            TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor,
                                            TObjectPtr<UTileSet3DAsset> InTileSet)
{
	TilemapEditorPtr = InTilemapEditor;
	TileSet = InTileSet;
	bEditProperty = false;
	CurrentFloor = 0;

	const UTilemap3DEditorSettings* Settings = GetMutableDefault<UTilemap3DEditorSettings>();
	CurrentTile = Settings->DefaultTileSet.LoadSynchronous()->TileSets[0];

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
				DrawEditStatusWidget()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			.Visibility_Lambda([this]()
			             {
				             return bEditProperty ? EVisibility::Visible : EVisibility::Hidden;
			             })
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Fill)
				  .Padding(2.0f)
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return FText::Format(LOCTEXT("TilemapFloorEntryLabel", "Floor: {0}"), CurrentFloor.Get());
					})
				]
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Fill)
				  .Padding(2.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					  .FillWidth(1.0f)
					  .HAlign(HAlign_Fill)
					  .VAlign(VAlign_Fill)
					[
						SNew(SButton)
						.OnClicked_Lambda([this]()
						{
							CurrentFloor = FMath::Max(CurrentFloor.Get() - 1, 0);
							FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(bEditProperty);
							return FReply::Handled();
						})
						[
							SNew(STextBlock)
							.Justification(ETextJustify::Center)
							.Text(LOCTEXT("TilemapFloorUpArrow", "<"))
						]
					]
					+ SHorizontalBox::Slot()
					  .FillWidth(1.0f)
					  .HAlign(HAlign_Fill)
					  .VAlign(VAlign_Fill)
					[
						SNew(SButton)
						.OnClicked_Lambda([this]()
						{
							CurrentFloor = FMath::Min(TilemapEditorPtr.Pin()->TilemapBeingEdited->Floors - 1,
							                          CurrentFloor.Get() + 1);
							FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(bEditProperty);
							return FReply::Handled();
						})
						[
							SNew(STextBlock)
							.Justification(ETextJustify::Center)
							.Text(LOCTEXT("TilemapFloorDownArrow", ">"))
						]
					]
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			[
				SNew(STileSetGalleyWidget)
				.TileSet(TileSet)
				.OnClicked_Lambda([this](const FName& ID)
				                          {
					                          CurrentTile = *TileSet->TileSets.FindByPredicate(
						                          [=](const FTileSet3DSubObject& Item)
						                          {
							                          return Item.ID == ID;
						                          });
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
				             return bEditProperty ? EVisibility::Visible : EVisibility::Hidden;
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
