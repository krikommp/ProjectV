// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SSkillEffectSection;
class SSkillEffectWindow;
namespace SkillEffect_Internal
{
	class FRowItemInfo
	{
		friend class SRowWidget;
		friend class ::SSkillEffectSection;
		
		FRowItemInfo(const FName& InName, const FName& InDescription);
		FRowItemInfo();

		FName Name;
		FName Description;
		bool bEmpty;
	};

	class SRowWidget : public SMultiColumnTableRow<TSharedPtr<FRowItemInfo>>
	{
	public:
		SLATE_BEGIN_ARGS(SRowWidget) {}
			SLATE_ARGUMENT(TSharedPtr<FRowItemInfo>, Item);
			SLATE_ARGUMENT(TWeakPtr<SSkillEffectSection>, OwnerWidget);
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView);

		virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

	public:
		TSharedPtr<FRowItemInfo> Item;
		TWeakPtr<SSkillEffectSection> OwnerWidget;
	};
}

/**
 * 
 */
class PROJECTVEDITOR_API SSkillEffectSection : public SCompoundWidget
{
	using ItemType = SkillEffect_Internal::FRowItemInfo;
	using ListViewType = SListView<TSharedPtr<ItemType>>;
	using WindowType = SSkillEffectWindow;
public:
	SLATE_BEGIN_ARGS(SSkillEffectSection)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	~SSkillEffectSection();

	TSharedRef<ITableRow> GenerateRow(TSharedPtr<ItemType> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnItemClicked(TSharedPtr<ItemType> InItem);
private:
	TArray< TSharedPtr<ItemType> > Items;
	TSharedPtr< ListViewType > ListWidget;
};
