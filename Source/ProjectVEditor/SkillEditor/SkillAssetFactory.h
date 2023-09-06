﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SkillAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVEDITOR_API USkillAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	//~ Begin UFactory interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	//~ Begin UFactory interface
};
