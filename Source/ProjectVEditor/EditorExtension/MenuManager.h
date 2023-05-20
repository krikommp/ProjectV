// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuItem/MenuItem.h"
#include "UObject/Object.h"
#include "MenuManager.generated.h"

/**
 * FMenuItemNode
 *
 * 菜单项节点
 * 用于构建菜单树
 */
struct FMenuItemNode
{
	/**
	 * @brief 菜单节点名称
	 */
	FString NodeName;

	/**
	 * @brief 菜单节点指针
	 */
	TSharedPtr<FMenuItem> MenuItem;

	/**
	 * @brief 菜单节点子节点
	 */
	TArray<FMenuItemNode> Children;
};

/**
 * UMenuManager
 *
 * 菜单管理器
 * 继承自 EngineSubsystem 以便在引擎加载 Module 时就进行加载
 */
UCLASS()
class PROJECTVEDITOR_API UMenuManager final : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	void AddMenuBarExtension(FMenuBarBuilder& Builder);

	/**
	 * @brief 添加菜单项到菜单树
	 * @param NewMenuItem 待插入的菜单节点
	 */
	void AddMenuItemToNodeList(const TSharedPtr<FMenuItem>& NewMenuItem);

	/**
	 * @brief 重新构建MenuItem
	 */
	void ReBuildMenuItemTree();

private:
	/**
	 * @brief 根节点数组，用于保存所有的MenuItem节点
	 */
	TArray<FMenuItemNode> RootNodeList;

	/**
	 * @brief 菜单扩展器
	 */
	TSharedPtr<FExtender> MenuExtender;
};
