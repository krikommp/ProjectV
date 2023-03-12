// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "UObject/Object.h"
#include "GridGameplayCueManager.generated.h"

/**
 * UGridGameplayCueManager
 *
 * 自定义 Gameplay Cues 管理器
 */
UCLASS()
class PROJECTVGAME_API UGridGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:

	UGridGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UGridGameplayCueManager* Get();

	void LoadAlwaysLoadedCues();
};
