// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapWarFogComponent.h"

#include "GridGlobalDelegates.h"
#include "GridMapManager.h"
#include "Engine/PostProcessVolume.h"
#include "Utility/Texture/TextureFunctionLibrary.h"


UGridMapWarFogComponent::UGridMapWarFogComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGridMapWarFogComponent::OnRegister()
{
	AGridMapManager* GridMapManager = GetPawnChecked<AGridMapManager>();
	GridMapManagerPtr = GridMapManager;

	// spawn a post process volume
	PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FTransform::Identity);
	PostProcessVolume->bUnbound = true;

	WarFogVisibleRangeTexture = UTextureFunctionLibrary::CreateTexture2D(this, GridMapManager->GridSizeX, GridMapManager->GridSizeY);
	UTextureFunctionLibrary::ClearTexture2D(WarFogVisibleRangeTexture, FLinearColor::Black);
	InitializeMaterial(PostProcessVolume);
	SetupMaterialParameters(WarFogVisibleRangeMaterial);
	DisableVisibleFog();

	FGridGlobalDelegates::OnRemoveVisibleRangeDisplay.AddUObject(this, &ThisClass::OnRemoveVisibleRangeDisplay);

	Super::OnRegister();
}

void UGridMapWarFogComponent::OnUnregister()
{
	FGridGlobalDelegates::OnRemoveVisibleRangeDisplay.RemoveAll(this);
	
	Super::OnUnregister();
}

void UGridMapWarFogComponent::EnableVisibleFog()
{
	check(GridMapManagerPtr.IsValid() && PostProcessVolume);

	PostProcessVolume->bEnabled = true;
}

void UGridMapWarFogComponent::DisableVisibleFog()
{
	check(GridMapManagerPtr.IsValid() && PostProcessVolume);

	PostProcessVolume->bEnabled = false;
}

void UGridMapWarFogComponent::PassVisibleRangeToMaterial(const TArray<int32>& InRangeArray)
{
	check(GridMapManagerPtr.IsValid());

	TextureData.Init(FLinearColor::Black, GridMapManagerPtr->GridSizeX * GridMapManagerPtr->GridSizeY);
	for(int32 Index = 0; Index < InRangeArray.Num(); ++Index)
	{
		if (InRangeArray[Index] != 0)
		{
			const int32 VisibleIndex = Index % (GridMapManagerPtr->GridSizeX * GridMapManagerPtr->GridSizeY);
			TextureData[VisibleIndex] = FLinearColor(GridMapManagerPtr->VectorFieldArray[VisibleIndex]);
		}
	}
	UTextureFunctionLibrary::SetVectorsDataToTexture2D(WarFogVisibleRangeTexture, TextureData);
}

void UGridMapWarFogComponent::InitializeMaterial(APostProcessVolume* InPostProcessVolume)
{
	WarFogVisibleRangeMaterial = UMaterialInstanceDynamic::Create(WarFogMaterial, this);
	K2_InitializeMaterial(InPostProcessVolume, WarFogVisibleRangeMaterial);
}

void UGridMapWarFogComponent::SetupMaterialParameters(UMaterialInstanceDynamic* InMaterial) const
{
	check(GridMapManagerPtr.IsValid() && PostProcessVolume);
	
	WarFogVisibleRangeMaterial->SetTextureParameterValue("WarFogVisibleRangeTexture", WarFogVisibleRangeTexture);
	WarFogVisibleRangeMaterial->SetVectorParameterValue("WarFogParam1", FLinearColor(GridMapManagerPtr->GridSizeX, GridMapManagerPtr->GridSizeY, GridMapManagerPtr->TileBoundsX * 0.5f, GridMapManagerPtr->TileBoundsY * 0.5f));
}

void UGridMapWarFogComponent::OnRemoveVisibleRangeDisplay()
{
	DisableVisibleFog();
}
