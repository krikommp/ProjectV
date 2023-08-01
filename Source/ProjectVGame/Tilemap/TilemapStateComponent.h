// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "UObject/Object.h"
#include "TilemapStateComponent.generated.h"

class AGridChessBase;
class UGridExperienceDefinition;
class ATilemap3DActor;
/**
 * UTilemapStateComponent
 *
 * 场景 Tilemap 管理器，用于生成地图数据
 */
UCLASS()
class PROJECTVGAME_API UTilemapStateComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	UTilemapStateComponent(const FObjectInitializer& ObjectInitializer);

	//~ Begin UActorComponent Interface.
	virtual void OnRegister() override;
	//~ Begin UActorComponent Interface.

	//~ Begin ILoadingProcessInterface Interface.
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~ Begin ILoadingProcessInterface Interface.
	
private:
	void OnExperienceLoaded(const UGridExperienceDefinition* Experience);
	void LoadTilemapFinished_Step1();
	void LoadTilemapFinished_Step2();
	
	// 需要被放置到场景中的 Tilemap
	UPROPERTY(Transient)
	TObjectPtr<ATilemap3DActor> TilemapActor;
	UPROPERTY(Transient)
	TArray<TObjectPtr<AGridChessBase>> TempChesses;

	// 是否 Tilemap 资源加载完毕
	bool bLoadTilemapFinished = false;
};
