// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapEditor.h"

#define LOCTEXT_NAMESPACE "UTilemapEditor"

static const FName TilemapTabName("Tilemap");
static const FText TilemapDisplayName = LOCTEXT("UTilemapEditor", "Tilemap Editor");

void UTilemapEditor::ShowWindow()
{
	Super::ShowWindow();
}

TSharedRef<SEditorWindowViewport> UTilemapEditor::CreateWindow()
{
	return Super::CreateWindow();
}

const FName& UTilemapEditor::GetTabName() const
{
	return TilemapTabName;
}

const FText& UTilemapEditor::GetDisplayName() const
{
	return TilemapDisplayName;
}

#undef LOCTEXT_NAMESPACE
