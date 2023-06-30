#pragma once
#include "EditorWindowExtension/EditorWindow/EditorWindowViewport.h"

class STilemapEditorViewport : public SEditorWindowViewport
{
public:
	SLATE_BEGIN_ARGS(STilemapEditorViewport)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	STilemapEditorViewport();
	virtual ~STilemapEditorViewport() override;
};
