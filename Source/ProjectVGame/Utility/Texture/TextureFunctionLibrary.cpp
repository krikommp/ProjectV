// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureFunctionLibrary.h"

UTexture2D* UTextureFunctionLibrary::CreateTexture2D(UObject* InOuter, int32 Width, int32 Height)
{
	UTexture2D* PseudoComputerTexture = NewObject<UTexture2D>(InOuter, NAME_None, RF_Transient);
	
	PseudoComputerTexture->SRGB = 0;
	PseudoComputerTexture->AddressX = TA_Clamp;
	PseudoComputerTexture->AddressY = TA_Clamp;
	PseudoComputerTexture->CompressionSettings = TC_HDR;
	PseudoComputerTexture->Filter = TF_Nearest;
	PseudoComputerTexture->MipGenSettings = TMGS_NoMipmaps;

	PseudoComputerTexture->Source.Init(Width, Height, 1, 1, TSF_RGBA16F);
	ClearTexture2D(PseudoComputerTexture, FLinearColor(1, 1, 1, 1));

	return PseudoComputerTexture;
}

void UTextureFunctionLibrary::ClearTexture2D(UTexture2D* Texture2D, const FLinearColor& ClearColor)
{
	const int32 DestBufferSize = Texture2D->Source.CalcMipSize(0);

	TArray<FFloat16Color> M_SrcData;
	M_SrcData.AddUninitialized(DestBufferSize);

	for (int32 Index = 0; Index < M_SrcData.Num(); ++Index)
	{
		M_SrcData[Index] = ClearColor;
	}

	uint32* DestBuffer = reinterpret_cast<uint32*>(Texture2D->Source.LockMip(0));
	FMemory::Memcpy(DestBuffer, M_SrcData.GetData(), DestBufferSize);

	Texture2D->Source.UnlockMip(0);
	Texture2D->UpdateResource();
}

void UTextureFunctionLibrary::SetVectorsDataToTexture2D(UTexture2D* Texture2D, const TArray<FLinearColor>& WriteData)
{
	if (Texture2D == nullptr) return;

	const int32 DestBufferSize = Texture2D->Source.CalcMipSize(0);

	TArray<FFloat16Color> M_SrcData;
	M_SrcData.Empty();
	M_SrcData.Init(FLinearColor(1, 1, 1, 1), DestBufferSize);

	for (int32 Index = 0; Index < WriteData.Num(); ++Index)
	{
		M_SrcData[Index] = WriteData[Index];
	}

	uint32* DestBuffer = reinterpret_cast<uint32*>(Texture2D->Source.LockMip(0));
	FMemory::Memcpy(DestBuffer, M_SrcData.GetData(), DestBufferSize);

	Texture2D->Source.UnlockMip(0);
	Texture2D->UpdateResource();
}
