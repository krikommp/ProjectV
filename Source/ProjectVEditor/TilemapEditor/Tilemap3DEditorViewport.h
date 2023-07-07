#pragma once
#include "EditorWindowExtension/EditorWindow/EditorWindowViewport.h"


class STilemap3DPropertiesTabBody;
class FTilemap3DEditorViewportClient;
class ULineBatchComponent;
class UTilemapAsset;
class FPreviewScene;
class UBoxComponent;
class STilemap3DPropertiesTabBody;

class STilemap3DEditorViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(STilemap3DEditorViewport)
		:_TilemapDetailPtr(MakeShareable((STilemap3DPropertiesTabBody*)nullptr))
		{
		}

	SLATE_ATTRIBUTE(TSharedPtr<STilemap3DPropertiesTabBody>, TilemapDetailPtr)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	STilemap3DEditorViewport();
	virtual ~STilemap3DEditorViewport() override;

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

	TSharedPtr<FPreviewScene> PreviewScene;

	TSharedPtr<FTilemap3DEditorViewportClient> TilemapAssetPreviewClient;

	TSharedPtr<STilemap3DPropertiesTabBody> DetailPtr;
};
