#pragma once
#include "EditorWindowExtension/EditorWindow/EditorWindowViewport.h"

class UTilemapAsset;

class STilemapEditorViewport : public SEditorWindowViewport
{
public:
	SLATE_BEGIN_ARGS(STilemapEditorViewport)
		:_TilemapBeingEdited((UTilemapAsset*)nullptr)
		{
		}

	SLATE_ATTRIBUTE(UTilemapAsset*, TilemapBeingEdited)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	STilemapEditorViewport();
	virtual ~STilemapEditorViewport() override;
};
