// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetFactory.h"

#include "..\..\ProjectVGame\Skill\SkillAsset.h"

USkillAssetFactory::USkillAssetFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USkillAsset::StaticClass();
}

UObject* USkillAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	USkillAsset* MyAsset = NewObject<USkillAsset>(InParent, InName, Flags | RF_Transactional);
	return MyAsset;
}
