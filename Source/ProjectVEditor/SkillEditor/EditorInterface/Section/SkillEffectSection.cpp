// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEffectSection.h"

#include "SlateOptMacros.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "SkillEditor/EditorInterface/Layout/WidgetLayoutUtils.h"
#include "SkillEditor/EditorInterface/Window/SkillEffectWindow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SSkillEffectSection"

namespace ViewRows
{
	static const FName TypeNameColumn(TEXT("TypeName"));
	static const FName DescriptionColumn(TEXT("Description"));
}

namespace SkillEffect_Internal
{
	FRowItemInfo::FRowItemInfo(const FName& InName, const FName& InDescription)
		: Name(InName), Description(InDescription), bEmpty(false)
	{
	}

	FRowItemInfo::FRowItemInfo()
		: Name(NAME_None), Description(NAME_None), bEmpty(true)
	{
	}

	void SRowWidget::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView)
	{
		Item = InArgs._Item;
		OwnerWidget = InArgs._OwnerWidget;

		check(Item.IsValid());

		SMultiColumnTableRow<TSharedPtr<FRowItemInfo>>::Construct(
			FSuperRowType::FArguments(),
			OwnerTableView);
	}

	TSharedRef<SWidget> SRowWidget::GenerateWidgetForColumn(const FName& ColumnName)
	{
		if (ColumnName == ViewRows::TypeNameColumn)
		{
			return SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(4).VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromName(Item->Name))
				];
		}
		else if (ColumnName == ViewRows::DescriptionColumn)
		{
			return SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(4).VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromName(Item->Description))
				];
		}

		return SNullWidget::NullWidget;
	}
}


void SSkillEffectSection::Construct(const FArguments& InArgs)
{
	using ThisType = TDecay<decltype(*this)>::Type;

	TSharedPtr<SVerticalBox> WidgetVerticalBox = SNew(SVerticalBox);
	TSharedPtr<SHorizontalBox> SkillEffectHBox_1 = SNew(SHorizontalBox);

	Items.Add(MakeShareable(new SkillEffect_Internal::FRowItemInfo(TEXT("附加状态"), TEXT("普通攻击 100%"))));
	Items.Add(MakeShareable(new SkillEffect_Internal::FRowItemInfo()));
	Items.Add(MakeShareable(new SkillEffect_Internal::FRowItemInfo()));
	Items.Add(MakeShareable(new SkillEffect_Internal::FRowItemInfo()));
	Items.Add(MakeShareable(new SkillEffect_Internal::FRowItemInfo()));

	TitleSlot(WidgetVerticalBox, LOCTEXT("SkillEffectTitleLabel", "Effect"));

	SAssignNew(ListWidget, ListViewType)
	.ListItemsSource(&Items)
	.OnGenerateRow(this, &ThisType::GenerateRow)
	.ItemHeight(22.0f)
	.SelectionMode(ESelectionMode::SingleToggle)
	.ListViewStyle(FAppStyle::Get(), "SimpleListView")
	.OnMouseButtonDoubleClick(this, &ThisType::OnItemClicked)
	.HeaderRow
	                                    (
		                                    SNew(SHeaderRow)
		                                    + SHeaderRow::Column(ViewRows::TypeNameColumn)
		                                    .DefaultLabel(LOCTEXT("SkillEffectTypeNameLabel", "Type"))

		                                    + SHeaderRow::Column(ViewRows::DescriptionColumn)
		                                    .DefaultLabel(LOCTEXT("SkillEffectDescriptionLabel", "Description"))
	                                    );

	NextHSlot(SkillEffectHBox_1)
	[
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "Button")
		.TextStyle(FAppStyle::Get(), "DialogButtonText")
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Text(LOCTEXT("SkillEffectAppandButtonLabel", "Append a new effect"))
	];

	NextHSlot(SkillEffectHBox_1)
	[
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "Button")
		.TextStyle(FAppStyle::Get(), "DialogButtonText")
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Text(LOCTEXT("SkillEffectDeleteButtonLabel", "Delete selected effect"))
	];

	NextVSlot(WidgetVerticalBox)
	[
		SkillEffectHBox_1.ToSharedRef()
	];

	NextVSlot(WidgetVerticalBox)
	[
		ListWidget.ToSharedRef()
	];

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.Header"), 6.0f))
		[
			WidgetVerticalBox.ToSharedRef()
		]
	];
}

SSkillEffectSection::~SSkillEffectSection()
{
}

TSharedRef<ITableRow> SSkillEffectSection::GenerateRow(TSharedPtr<ItemType> InItem,
                                                       const TSharedRef<STableViewBase>& OwnerTable)
{
	check(InItem.IsValid());

	using SRowWidget = SkillEffect_Internal::SRowWidget;

	return SNew(SRowWidget, OwnerTable)
		.Item(InItem)
		.OwnerWidget(SharedThis(this));
}

void SSkillEffectSection::OnItemClicked(TSharedPtr<ItemType> InItem)
{
	TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
	TSharedPtr<WindowType> Window = SNew(WindowType)
		.ParentWindow(RootWindow);
	
	if (RootWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), RootWindow.ToSharedRef());
	}else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
