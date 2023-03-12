// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GridExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * UGridExperienceActionSet
 *
 * 定义一组进入 Experience 需要执行的 Actions
 */
UCLASS(BlueprintType, NotBlueprintable)
class PROJECTVGAME_API UGridExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGridExperienceActionSet();

	// ~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif
	// ~UObject interface

	// ~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	// ~UPrimaryDataAsset interface

public:

	// 定义一组需要被 Experience 执行 loaded/activated/deactivated/unloaded 操作的 Actions 列表
	UPROPERTY(EditAnywhere, Instanced, Category="Action to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// 定义一组需要被激活的 GameFeature
	UPROPERTY(EditAnywhere, Category="Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;
};
