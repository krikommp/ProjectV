// Fill out your copyright notice in the Description page of Project Settings.


#include "TilemapAssetFactory.h"

#include "Tilemap/TilemapAsset.h"

UTilemapAssetFactory::UTilemapAssetFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTilemapAsset::StaticClass();
}

UObject* UTilemapAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UTilemapAsset* MyAsset = NewObject<UTilemapAsset>(InParent, InName, Flags | RF_Transactional);
	return MyAsset;
}
