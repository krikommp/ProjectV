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
 * 用于计算棋子当前可见范围
 * 挂载到需要计算可见范围的物体上
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UTilemapVisibleComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UTilemapVisibleComponent* FindTilemapVisibleComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UTilemapVisibleComponent>() : nullptr);
	}
	
protected:
	virtual void BeginPlay() override;

public:

private:
};
