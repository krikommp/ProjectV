// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Array.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/HUD.h"
#include "UObject/UObjectGlobals.h"

#include "GridHUD.generated.h"

class AActor;
class UObject;

/**
 * AGridHUD
 *
 *  Note that you typically do not need to extend or modify this class, instead you would
 *  use an "Add Widget" action in your experience to add a HUD layout and widgets to it
 * 
 *  This class exists primarily for debug rendering
 */
UCLASS(Config = Game)
class PROJECTVGAME_API AGridHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGridHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AHUD interface
	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;
	//~End of AHUD interface
};
