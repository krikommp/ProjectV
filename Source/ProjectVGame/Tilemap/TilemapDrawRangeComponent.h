// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "TilemapDrawRangeComponent.generated.h"

/**
 * UTilemapDrawRangeComponent
 *
 * 绘制范围组件
 * 挂载到需要显示范围的物体上
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UTilemapDrawRangeComponent : public UPawnComponent
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UTilemapDrawRangeComponent* FindTilemapDrawRangeComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UTilemapDrawRangeComponent>() : nullptr);
	}

protected:
	virtual void BeginPlay() override;

public:
	// 绘制寻路贴画
	UFUNCTION(BlueprintCallable)
	void DisplayPathfindingDecal(TArray<int32> Indexes);

	// 清理所有缓存的贴花
	UFUNCTION(BlueprintCallable)
	void ClearAllDecalComponents();

private:
	// 记录生成的贴画组件
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDecalComponent>> DecalComponents;

	// 贴画大小
	const FVector DecalSize = { 95.0f, 45.0f, 45.0f };
};
