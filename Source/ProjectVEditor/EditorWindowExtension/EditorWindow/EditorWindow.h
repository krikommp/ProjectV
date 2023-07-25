// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EditorWindow.generated.h"

class SEditorWindowViewport;

/**
 * UEditorWindow
 *
 * 自定义编辑器窗口
 *\code
 * UCLASS()
 * class PROJECTVEDITOR_API UTestEditorWindow : public UEditorWindow
 * {
 *	    GENERATED_BODY()
 *  public:
 *	    UFUNCTION(meta=(MenuItem="FeelingSoul/Test Editor", ToolTip="Open Test Editor"))
 *	    virtual void ShowWindow() override;
 *
 *	    virtual TSharedRef<SEditorWindowViewport> CreateWindow() override;
 * };
 *\endcode 
 */
UCLASS()
class PROJECTVEDITOR_API UEditorWindow : public UObject
{
	GENERATED_BODY()

public:
	virtual const FName& GetTabName() const;
	virtual const FText& GetDisplayName() const;
	virtual void ShowWindow();
	virtual TSharedRef<SEditorWindowViewport> CreateWindow();

	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
};