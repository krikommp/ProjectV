// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FMenuItem : public TSharedFromThis<FMenuItem>
{
private:
	/**
	 * @brief 函数持有者
	 */
	TWeakObjectPtr<UObject> Owner = nullptr;

	/**
	 * @brief 被调用函数名称
	 */
	FName FunctionName;

	/**
	 * @brief 菜单项路径，例： "First/Second/Third"
	 */
	FString MenuPath;

	/**
	 * @brief 菜单名称, 例：Third
	 */
	FString MenuName;

	/**
	 * @brief 菜单项提示
	 */
	FString MenuToolTip;

	/**
	 * @brief 是否为叶子节点
	 */
	bool bLeaf = false;

public:
	const FString& GetMenuPath() const { return MenuPath; }
	const FString& GetMenuName() const { return MenuName; }
	const FString& GetMenuToolTip() const { return MenuToolTip; }
	bool IsLeaf() const { return bLeaf; }

public:
	/**
	 * @brief 创建菜单项
	 * @param Path 菜单路径
	 * @param Tooltip 菜单提示项
	 * @param InOwner 函数持有者
	 * @param InFuncName 被调用函数名称
	 */
	void InitMenu(const FString& Path, const FString& Tooltip, UObject* InOwner = nullptr, const FName& InFuncName = FName());

	void OnMenuClick() const;
};
