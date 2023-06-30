// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorWindowExtension/EditorWindow/EditorWindow.h"
#include "UObject/Object.h"
#include "TilemapEditor.generated.h"

/**
 * UTilemapEditor
 *
 * Tilemap 编辑器
 */
UCLASS()
class PROJECTVEDITOR_API UTilemapEditor : public UEditorWindow
{
	GENERATED_BODY()
public:
	UFUNCTION(meta=(MenuItem="FeelingSoul/Tilemap Editor", ToolTip="打开 Tilemap 编辑器"))
	virtual void ShowWindow() override;
	virtual TSharedRef<SEditorWindowViewport> CreateWindow() override;

	virtual const FName& GetTabName() const override;
	virtual const FText& GetDisplayName() const override;
};
