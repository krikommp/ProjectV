// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityEffects/GridGameplayEffect.h"
#include "UObject/Object.h"
#include "SkillEffect.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTVGAME_API USkillEffect : public UGridGameplayEffect
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FName Name;
};
