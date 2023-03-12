// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWorldSettings.generated.h"

class UGridExperienceDefinition;

/**
 * AGridWorldSettings
 *
 * 项目自定义 WorldSettings
 * 可以用于指定关卡默认 Experience
 */
UCLASS()
class PROJECTVGAME_API AGridWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	AGridWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

public:
	// 如果没有从 UserFacingExperience 中重载 Experience, 那么就从这里获取进入 Map 后的默认加载 Experience
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UGridExperienceDefinition> DefaultGameplayExperience;

public:
#if WITH_EDITORONLY_DATA
	// Is this level part of a front-end or other standalone experience?
	// When set, the net mode will be forced to Standalone when you hit Play in the editor
	UPROPERTY(EditDefaultsOnly, Category=PIE)
	bool ForceStandaloneNetMode = false;
#endif
};
