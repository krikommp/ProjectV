#include "Tilemap3DPropertiesTabBody.h"

#include "Tilemap3DEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"
#include "Tilemap3DEditorViewport.h"
#include "Tilemap3DEditorManager.h"

#define LOCTEXT_NAMESPACE "STilemap3DPropertiesTabBody"

void STilemap3DPropertiesTabBody::Construct(const FArguments& InArgs, TSharedPtr<FTilemap3DEditorToolkit> InTilemapEditor)
{
	TilemapEditorPtr = InTilemapEditor;

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
		return !TilemapEditorPtr.Pin()->bEditProperty;
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
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Fill)
				  .Padding(2.0f)
				[
					SNew(SButton)
					.OnClicked_Lambda([this]()
					{
						this->TilemapEditorPtr.Pin()->bEditProperty = !this->TilemapEditorPtr.Pin()->bEditProperty;
						this->EditStatusText->SetText(this->TilemapEditorPtr.Pin()->bEditProperty
							                              ? LOCTEXT("Stop", "Stop Editing")
							                              : LOCTEXT("Start", "Start Editing"));
						FTilemap3DEditDelegates::OnTilemapEditStatueChanged.Broadcast(this->TilemapEditorPtr.Pin()->bEditProperty);
						return FReply::Handled();
					})
					[
						SAssignNew(EditStatusText, STextBlock)
						.Justification(ETextJustify::Center)
						.Text(this->TilemapEditorPtr.Pin()->bEditProperty
							      ? LOCTEXT("Stop", "Stop Editing")
							      : LOCTEXT("Start", "Start Editing"))
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
