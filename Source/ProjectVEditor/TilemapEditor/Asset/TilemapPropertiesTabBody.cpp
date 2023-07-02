#include "TilemapPropertiesTabBody.h"

#include "TilemapEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"

#define LOCTEXT_NAMESPACE "STilemapPropertiesTabBody"

void STilemapPropertiesTabBody::Construct(const FArguments& InArgs, TSharedPtr<FTilemapEditorToolkit> InTilemapEditor)
{
	TilemapEditorPtr = InTilemapEditor;

	SSingleObjectDetailsPanel::Construct(
		SSingleObjectDetailsPanel::FArguments().HostCommandList(InTilemapEditor->GetToolkitCommands()).
		                                        HostTabManager(InTilemapEditor->GetTabManager()),
		/*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/ true);
}

UObject* STilemapPropertiesTabBody::GetObjectToObserve() const
{
	return TilemapEditorPtr.Pin()->GetTilemapBeingEdited();
}

TSharedRef<SWidget> STilemapPropertiesTabBody::PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget)
{
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
			.Visibility_Lambda([this]() { return EVisibility::Visible; })
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(2.0f)
				[
					SNew(SButton)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NewLevel", "Set Up New Level Spawner"))
					]
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(2.0f)
				[
					SNew(SButton)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("FindLevel", "Find New Level Spawner"))
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
