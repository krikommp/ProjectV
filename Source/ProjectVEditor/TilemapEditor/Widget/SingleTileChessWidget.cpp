// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleTileChessWidget.h"

#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSingleTileChessWidget::Construct(const FArguments& InArgs, const FGridHeroData& InHeroData)
{
	ChessData = InHeroData;
	OnTileSetClicked = InArgs._OnTileSetClicked;

	UTexture2D* PreviewTex = GetThumbnailFromUSkeletalMesh(InHeroData.SkeletalMesh);
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
			.Text(ChessData.HeroName)
			.OnClicked_Lambda([this]()
			             {
				             // ReSharper disable once CppExpressionWithoutSideEffects
				             OnTileSetClicked.ExecuteIfBound(ChessData.HeroID);
				             return FReply::Handled();
			             })
		]
	];
}

UTexture2D* SSingleTileChessWidget::GetThumbnailFromUSkeletalMesh(USkeletalMesh* Mesh) const
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
