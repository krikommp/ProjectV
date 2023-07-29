// Fill out your copyright notice in the Description page of Project Settings.


#include "Tilemap3DAssetFactory.h"

#include "Tilemap/TilemapAsset.h"

UTilemapAsset3DFactory::UTilemapAsset3DFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTilemapAsset::StaticClass();
}

UObject* UTilemapAsset3DFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UTilemapAsset* MyAsset = NewObject<UTilemapAsset>(InParent, InName, Flags | RF_Transactional);
	return MyAsset;
}
