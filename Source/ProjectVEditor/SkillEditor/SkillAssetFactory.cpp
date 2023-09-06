// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetFactory.h"

#include "Skill/SkillBaseAsset.h"

USkillAssetFactory::USkillAssetFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USkillBaseAsset::StaticClass();
}

UObject* USkillAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	USkillBaseAsset* MyAsset = NewObject<USkillBaseAsset>(InParent, InName, Flags | RF_Transactional);
	return MyAsset;
}
