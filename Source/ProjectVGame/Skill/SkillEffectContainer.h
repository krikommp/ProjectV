// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SkillEffectContainer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API USkillEffectContainer : public UObject
{
	GENERATED_BODY()

public:
	// 技能效果类型
	UPROPERTY()
	FString Type;

	// 技能效果描述
	UPROPERTY()
	FString Description;
};
