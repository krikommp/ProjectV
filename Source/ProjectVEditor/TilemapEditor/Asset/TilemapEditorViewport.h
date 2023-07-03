#pragma once
#include "EditorWindowExtension/EditorWindow/EditorWindowViewport.h"


class FTilemapEditorViewportClient;
class ULineBatchComponent;
class UTilemapAsset;
class FPreviewScene;
class UBoxComponent;

class STilemapEditorViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
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

	// ICommonEditorViewportToolbarInfoProvider interface
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;
	// End of ICommonEditorViewportToolbarInfoProvider interface

	// SEditorViewport interface
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	// End of SEditorViewport interface

protected:
	/** SEditorViewport interface */
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

	TObjectPtr<UTilemapAsset> TilemapBeingEdited;

	TSharedPtr<FPreviewScene> PreviewScene;

	TSharedPtr<FTilemapEditorViewportClient> TilemapAssetPreviewClient;
};
