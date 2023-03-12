// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "GridAbilityBuffUIData.generated.h"

class UTexture2D;

/**
 * UGridAbilityBuffUIData
 *
 * Buff UIData
 */
UCLASS()
class PROJECTVGAME_API UGridAbilityBuffUIData : public UGameplayEffectUIData
{
	GENERATED_UCLASS_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	TObjectPtr<UTexture2D> BuffIcon;
};
