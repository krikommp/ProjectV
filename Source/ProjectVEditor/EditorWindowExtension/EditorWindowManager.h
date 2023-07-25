// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EditorWindowManager.generated.h"

/**
 * UEditorWindowManager
 *
 * 编辑器窗口管理器
 */
UCLASS()
class PROJECTVEDITOR_API UEditorWindowManager final : public UEngineSubsystem 
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UEditorWindowManager* Get();
protected:
	/**
	 * @brief 構建編輯器窗口
	 */
	void RebuildEditorWindows();


private:
	/**
	 * @brief 所有显示的编辑器窗口ID
	 */
	TArray<FName> TabNames;
};
