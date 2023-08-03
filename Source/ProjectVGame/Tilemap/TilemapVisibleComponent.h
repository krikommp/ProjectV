// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "TilemapVisibleComponent.generated.h"

/**
 * UTilemapVisibleComponent
 *
 * 地图可见性组件
 * 实现显示移动范围，可视范围等功能
 * 挂载到需要显示范围的物体上
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UTilemapVisibleComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()
	FIND_PAWN_COMP_FUNC(TilemapVisibleComponent)
	
protected:
	virtual void BeginPlay() override;

public:
	// 绘制寻路贴画
	void DisplayPathfindingDecal(int32 Index, UMaterialInterface* Material);
	// 清理所有缓存的贴花
	void ClearAllDecalComponents();

private:
	// 记录生成的贴画组件
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDecalComponent>> DecalComponents;

	// 贴画大小
	const FVector DecalSize = { 100.0f, 50.0f, 50.0f };
};
