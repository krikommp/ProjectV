// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillIconWindow.h"

#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillIconWindow"

void SSkillIconWindow::Construct(const FArguments& InArgs)
{
	using ThisType = TDecay<decltype(*this)>::Type;
	TArray<TObjectPtr<UTexture2D>> IconTexArray;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");
	FARFilter Filter;
	Filter.ClassPaths.Add(UTexture2D::StaticClass()->GetClassPathName());
	Filter.PackagePaths.Add(TEXT("/Game"));
	Filter.bRecursivePaths = true;
	TArray<FAssetData> OutAssets;
	AssetRegistryModule.Get().GetAssets(Filter, OutAssets);
	for (const FAssetData& Asset : OutAssets)
	{
		FString AssetPath = Asset.GetObjectPathString();
		UTexture2D* LoadedAsset = LoadObject<UTexture2D>(nullptr, *AssetPath);
		IconTexArray.Add(LoadedAsset);
	}

	TSharedRef<SGridPanel> GridPanel = SNew(SGridPanel)
		.FillColumn(0, 0.0f)
		.FillColumn(1, 0.0f)
		.FillColumn(2, 0.0f)
		.FillColumn(3, 0.0f)
		.FillColumn(4, 0.0f)
		.FillColumn(5, 0.0f);

	for (int32 i = 0; i < IconTexArray.Num(); i++)
	{
		FSlateBrush* PreviewBrush = new FSlateBrush;
		PreviewBrush->SetResourceObject(IconTexArray[i].Get());
		PreviewBrush->SetImageSize({64, 64});
		PreviewBrush->DrawAs = ESlateBrushDrawType::Image;

		TSharedRef<SImage> ImageWidget = SNew(SImage)
			.Image(PreviewBrush)
			.OnMouseButtonDown_Raw(this, &ThisType::OnImageClicked);

		int32 RowIndex = i / 6;
		int32 ColumnIndex = i % 6;

		GridPanel->AddSlot(ColumnIndex, RowIndex)
		         .Padding(4)
		         .HAlign(HAlign_Center)
		         .VAlign(VAlign_Center)
		[
			ImageWidget
		];
	}

	SWindow::Construct(SWindow::FArguments()
	                   .Title(LOCTEXT("SkillIconSelectorTitleLabel", "IconSelector"))
	                   .SupportsMinimize(false)
	                   .SupportsMaximize(false)
	                   .AutoCenter(EAutoCenter::PreferredWorkArea)
	                   .ClientSize(FVector2D(450, 450))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(2)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						GridPanel
					]
				]
			]
		]);
}

FReply SSkillIconWindow::OnImageClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	RequestDestroyWindow();
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
