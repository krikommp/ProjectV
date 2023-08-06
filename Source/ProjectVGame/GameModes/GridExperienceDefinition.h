// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridExperienceDefinition.generated.h"

class UGridPawnData;
class UGameFeatureAction;
class UGridExperienceActionSet;
class UTilemapAsset;

/**
 * UGridExperienceDefinition
 *
 */
UCLASS(BlueprintType, Const)
class PROJECTVGAME_API UGridExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGridExperienceDefinition();

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

	// 定义一组该 Experience 中需要激活的 GameFeature
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<FString> GameFeatureToEnable;

	// player 的默认 PawnData
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UGridPawnData> DefaultPawnData;

	// 定义一组需要被 Experience 执行 loaded/activated/deactivated/unloaded 操作的 Actions 列表
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// 定义一组额外的 Action sets
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<UGridExperienceActionSet>> ActionSets;

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UTilemapAsset> TilemapAsset;
};
