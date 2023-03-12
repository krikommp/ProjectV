// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridAITargetType.generated.h"

/**
 * UGridAITargetType
 *
 * 定义AI查找目标的方法
 */
UCLASS(BlueprintType, Blueprintable, meta=(ShowWorldContextPin))
class PROJECTVGAME_API UGridAITargetType : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Grid|AI", meta=(WorldContext = "WorldContextObject"))
	int32 GetTargets(const UObject* WorldContextObject, AActor* OwnerActor, const TArray<int32>& SearchableIndexArray);
};

/**
 * UGridAITargetType_User
 *
 * 作用目标是自己
 */
UCLASS(NotBlueprintType)
class PROJECTVGAME_API UGridAITargetType_User : public UGridAITargetType
{
	GENERATED_BODY()
public:
	virtual int32 GetTargets_Implementation(const UObject* WorldContextObject, AActor* OwnerActor, const TArray<int32>& SearchableIndexArray) override;
};



