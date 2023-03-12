// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridAIFunctionLibrary.generated.h"

class UGridAITargetType;

/**
 * UGridAIFunctionLibrary
 *
 * AI系统辅助函数
 */
UCLASS()
class PROJECTVGAME_API UGridAIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// 获取 AITargetType 对象
	UFUNCTION(BlueprintCallable, Category="Grid|AI")
	static UGridAITargetType* GetAITargetType(TSubclassOf<UGridAITargetType> AITargetType);
};
