// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "TilemapDrawRangeComponent.generated.h"

class USplineComponent;
class USplineMeshComponent;

/**
 * UTilemapDrawRangeComponent
 *
 * 绘制范围组件
 * 挂载到需要显示范围的物体上
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UTilemapDrawRangeComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UTilemapDrawRangeComponent* FindTilemapDrawRangeComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UTilemapDrawRangeComponent>() : nullptr);
	}

	// 绘制寻路贴画
	UFUNCTION(BlueprintCallable)
	void DisplayPathfindingDecal(TArray<int32> Indexes);
	// 清理所有缓存的贴花
	UFUNCTION(BlueprintCallable)
	void ClearAllDecalComponents();

	// 绘制寻路路径
	UFUNCTION(BlueprintCallable)
	void DisplayPathfindingSplinePath(const TArray<int32>& PathIndexArray);
	// 清理所有缓存的路径
	UFUNCTION(BlueprintCallable)
	void ClearAllSplinePath();
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TArray<FVector> CreateSplinePath(const TArray<int32>& PathIndexArray) const;
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

private:
	// 记录生成的贴画组件
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDecalComponent>> DecalComponents;
	// 用于显示寻路路径的组件
	UPROPERTY(Transient)
	TObjectPtr<USplineComponent> DisplaySplineComponent;
	// 记录显示路径点
	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> DisplayRoadMeshComponents;
	// 贴画大小
	const FVector DecalSize = { 95.0f, 45.0f, 45.0f };
	// 模块名称
	static const FName NAME_ActorFeatureName;
};
