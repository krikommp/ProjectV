// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "UObject/Object.h"
#include "TilemapSpawnParameters.h"
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

	// 是否 Tilemap 资源加载完毕
	bool bLoadTilemapFinished = false;
	
public:
	// 注册或调用棋子放置的委托
	void CallOrRegister_OnChessSpawn(FOnTilemapSpawnChess::FDelegate&& Delegate);

	// 注册或调用地图放置完成委托
	void CallOrRegister_OnSpawnFinished(FSimpleMulticastDelegate::FDelegate&& Delegate);

private:
	// 当放置棋子时调用的委托
	FOnTilemapSpawnChess OnTilemapSpawnChess;

	// 缓存的放置信息
	TArray<FTilemapSpawnParameters> CachedSpawnParameters;

	// 地图准备完成
	FSimpleMulticastDelegate OnTilemapSpawnFinished;
};
