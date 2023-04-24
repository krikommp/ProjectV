// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TextureFunctionLibrary.generated.h"

/**
 * UTextureFunctionLibrary
 * 创建及写入 Texture2D 的辅助方法
 */
UCLASS()
class PROJECTVGAME_API UTextureFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UTexture2D* CreateTexture2D(UObject* InOuter, int32 Width, int32 Height);
	
	UFUNCTION(BlueprintCallable)
	static void ClearTexture2D(UTexture2D* Texture2D, const FLinearColor& ClearColor);

	UFUNCTION(BlueprintCallable)
	static void SetVectorsDataToTexture2D(UTexture2D* Texture2D, const TArray<FLinearColor>& WriteData);
};
