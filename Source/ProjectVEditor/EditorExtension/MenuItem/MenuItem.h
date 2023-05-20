// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MenuItem.generated.h"

/**
 * UMenuItem
 *
 * 作为所有MenuItem类的基类
 */
UCLASS(Abstract)
class PROJECTVEDITOR_API UMenuItem : public UObject
{
	GENERATED_BODY()

public:
	~UMenuItem() override = default;

	virtual void OnMenuClick();

private:
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

public:
	const FString& GetMenuPath() const { return MenuPath; }
	const FString& GetMenuName() const { return MenuName; }
	const FString& GetMenuToolTip() const { return MenuToolTip; }

protected:
	/**
	 * @brief 创建菜单项
	 * @param Path 菜单路径
	 * @param Tooltip 菜单提示项
	 */
	void InitMenu(const FString& Path, const FString& Tooltip);
};
