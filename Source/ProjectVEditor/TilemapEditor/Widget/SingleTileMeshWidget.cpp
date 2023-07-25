// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleTileMeshWidget.h"

#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSingleTileMeshWidget::Construct(const FArguments& InArgs, const FTileSet3DMesh& InTileSet)
{
	TileSet3DMesh = InTileSet;
	OnTileSetClicked = InArgs._OnTileSetClicked;

	UTexture2D* PreviewTex = GetThumbnailFromStaticMesh(TileSet3DMesh.Mesh);
	ensureAlwaysMsgf(PreviewTex, TEXT("Can not get thumbnail texture，check config."));

	PreviewBrush = MakeShareable(new FSlateBrush);
	PreviewBrush->SetResourceObject(PreviewTex);
	PreviewBrush->SetImageSize({64, 64});
	PreviewBrush->DrawAs = ESlateBrushDrawType::Image;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SImage)
			.Image(PreviewBrush.Get())
		]
		+ SVerticalBox::Slot()
		  .Padding(0.0f, 10.0f)
		  .AutoHeight()
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnTileSetClicked.ExecuteIfBound(TileSet3DMesh.ID);
				return FReply::Handled();
			})
			[
				SNew(STextBlock)
			.Justification(ETextJustify::Center)
			.Text(FText::FromName(TileSet3DMesh.ID))
			]
		]
	];
}

UTexture2D* SSingleTileMeshWidget::GetThumbnailFromStaticMesh(UStaticMesh* Mesh) const
{
	UTexture2D* Texture = nullptr;

	const FAssetData AssetData = FAssetData(Mesh);
	const FName ObjectFullName = FName(*AssetData.GetFullName());
	FString PackageFileName;
	TSet<FName> ObjectFullNames;
	ObjectFullNames.Add(ObjectFullName);
	if (FPackageName::DoesPackageExist(AssetData.PackageName.ToString(), &PackageFileName))
	{
		FThumbnailMap ThumbnailMap;
		ThumbnailTools::LoadThumbnailsFromPackage(PackageFileName, ObjectFullNames, ThumbnailMap);
		FObjectThumbnail* ObjTN = ThumbnailMap.Find(ObjectFullName);

		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(
			TEXT("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

		ImageWrapper->SetRaw(ObjTN->GetUncompressedImageData().GetData(), ObjTN->GetUncompressedImageData().Num(),
		                     ObjTN->GetImageWidth(), ObjTN->GetImageHeight(), ERGBFormat::BGRA, 8);
		const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();

		Texture = FImageUtils::ImportBufferAsTexture2D(CompressedByteArray);
	}

	return Texture;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
