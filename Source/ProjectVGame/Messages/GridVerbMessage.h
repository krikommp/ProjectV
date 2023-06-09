﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GridVerbMessage.generated.h"

/**
 * FGridVerbMessage
 *
 * 动作消息，带有发起者的上下文信息
 */
USTRUCT(BlueprintType)
struct FGridVerbMessage
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTag Verb;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	TObjectPtr<UObject> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	TObjectPtr<UObject> Target = nullptr;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTagContainer InstigatorTags;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTagContainer TargetTags;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	double Magnitude = 1.0f;

	PROJECTVGAME_API FString ToString() const;
};
