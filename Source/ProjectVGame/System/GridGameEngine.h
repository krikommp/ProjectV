// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameEngine.h"
#include "UObject/UObjectGlobals.h"
#include "GridGameEngine.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API UGridGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UGridGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};
