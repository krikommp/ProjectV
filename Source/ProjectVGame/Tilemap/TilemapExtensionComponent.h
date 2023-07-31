// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Tilemap3DActor.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "TilemapExtensionComponent.generated.h"


class ATilemap3DActor;
/**
 * UTilemapExtensionComponent
 *
 * 对于需要棋盘信息的 Pawn 都要挂载这个组件
 * 用来获取 Tilemap 信息
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UTilemapExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()
	FIND_COMP(TilemapExtensionComponent)
	
	// 定义该模块的名称
	static const FName NAME_ActorFeatureName;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
									FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
									   FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

	void SetTilemap(const ATilemap3DActor* InTilemap);
	const ATilemap3DActor* GetTilemap() const { return Tilemap3DActor.Get(); }
	
private:
	// 弱引用场景中的Tilemap
	TWeakObjectPtr<const ATilemap3DActor> Tilemap3DActor;
};
