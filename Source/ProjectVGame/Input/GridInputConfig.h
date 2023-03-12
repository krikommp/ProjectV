// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Array.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UObject/UObjectGlobals.h"
#include "GridInputConfig.generated.h"

class UInputAction;
class UObject;
struct FFrame;

/**
 * FGridInputAction
 *
 * 映射输入和 GameplayTag 的结构
 */
USTRUCT(BlueprintType)
struct FGridInputAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * UGridInputConfig
 *
 * 输入配置属性的不变资产
 */
UCLASS(BlueprintType, Const)
class PROJECTVGAME_API UGridInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UGridInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Grid|Pawn")
const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Grid|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FGridInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FGridInputAction> AbilityInputActions;
};
