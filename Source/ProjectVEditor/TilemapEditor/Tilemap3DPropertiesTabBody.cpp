#include "Tilemap3DPropertiesTabBody.h"

#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorViewport.h"
#include "Tilemap3DEditorManager.h"

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
                                            TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor)
{
	TilemapEditorPtr = InTilemapEditor;
	bEditProperty = false;
	CurrentFloor = 0;

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
							CurrentFloor = FMath::Min(TilemapEditorPtr.Pin()->TilemapBeingEdited->Floors,
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

		];
}

#undef LOCTEXT_NAMESPACE
