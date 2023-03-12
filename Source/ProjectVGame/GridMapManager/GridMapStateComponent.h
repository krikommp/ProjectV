// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GridMapStateComponent.generated.h"

class AGridMapManager;
class UGridExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridMapMangerInitielized, AGridMapManager* GridMapManager);

/**
 * UGridMapStateComponent
 *
 * 地图管理器组件
 */
UCLASS()
class PROJECTVGAME_API UGridMapStateComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UGridMapStateComponent(const FObjectInitializer& ObjectInitializer);

	// ~UActorComponent interface
	virtual void OnRegister() override;
	// ~UActorComponent interface

	AGridMapManager& GetGGridMapManagerRef() const { check(GridMapManager); return *GridMapManager; }
	
	UFUNCTION(BlueprintPure, Category="Grid|GridMap")
	AGridMapManager* GetGridMapManager() const { return GridMapManager; }

	void CallOrRegister_OnGridMapInitialized(FOnGridMapMangerInitielized::FDelegate&& Delegate);
	
protected:

	void OnExperienceLoaded(const UGridExperienceDefinition* Experience);

	void ActivateGridMap();

private:

	UPROPERTY(Transient)
	TObjectPtr<AGridMapManager> GridMapManager;

	FOnGridMapMangerInitielized OnGridMapInitialized;

	bool bGridMapInitialized = false;
};
