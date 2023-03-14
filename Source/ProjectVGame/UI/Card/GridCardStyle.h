// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridCardStyle.generated.h"

/**
 * UGridCardStyle
 */
UCLASS(Blueprintable, EditInlineNew, CollapseCategories)
class PROJECTVGAME_API UGridCardStyle : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	FLinearColor FrameColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	FSlateColor CostColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	float Opacity;
};
