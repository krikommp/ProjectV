#include "TilemapPropertiesTabBody.h"

#include "TilemapEditorToolkit.h"
#include "Tilemap/TilemapAsset.h"
#include "TilemapEditorViewport.h"

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
						this->TilemapEditorPtr.Pin()->ViewportPtr->DrawGrid(FVector::Zero(), 5, 5, 100, 100, FLinearColor::Red, 2.0f);
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
