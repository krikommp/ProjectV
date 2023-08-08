// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponents.h"
#include "Tilemap3DActor.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "TilemapExtensionComponent.generated.h"


struct FTilemapSpawnParameters;
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

	UFUNCTION(BlueprintPure)
	static UTilemapExtensionComponent* FindTilemapExtensionComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UTilemapExtensionComponent>() : nullptr);
	}
	
	// 定义该模块的名称
	static const FName NAME_ActorFeatureName;

	// 检测传入位置是否越界, 如果越界将会返回修正之后的位置，否则返回原始位置
	void CheckLocationOutBound(FVector& InLocation) const;
	// 检测传入位置索引上是否有棋子
	FORCEINLINE bool CheckChessExist(int32 Index) const;
	// 获取寻路数组总大小
	FORCEINLINE int32 GetPathfindingArrayNum() const;
	// 获取寻路方块
	FORCEINLINE const FTilemapPathFindingBlock& GetPathfindingBlock(int32 Index) const;
	// 获取寻路索引
	FORCEINLINE int32 GetPathfindingIndex() const { return PathfindingIndex; }
	// 设置寻路索引
 	void SetPathfindingIndex(int32 Index);
	// 获取寻路节点位置
	FORCEINLINE FVector GetPathfindingLocation(int32 Index, float ZOffset) const;
	// 获取寻路节点的位置
	FORCEINLINE FVector GetPathfindingBlockLocation(int32 Index) const;
	// 根据寻路节点获取block信息
	UBlock* GetBlockFromPathfindingIndex(int32 Index) const;
	
	// 将位置转换为寻路索引
	UFUNCTION(BlueprintPure)
	int32 LocationToPathfindingIndex(const FVector& Location) const;
	// 获取鼠标点击地图位置和索引
	UFUNCTION(BlueprintPure)
	bool GetHitLocationAndIndex(ETraceTypeQuery TraceChannel, int32& OutIndex, FVector& OutLocation) const;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnChessSpawn(const FTilemapSpawnParameters& Parameters);

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

	void SetTilemap(ATilemap3DActor* InTilemap, int32 InPathfindingIndex = INDEX_NONE);
	const UTilemapAsset* GetTilemap() const { return Tilemap3DActor->GetTilemap(); }
	ATilemap3DActor* GetTilemapActor() const { return Tilemap3DActor.Get(); }
	
private:
	// 弱引用场景中的Tilemap
	TWeakObjectPtr<ATilemap3DActor> Tilemap3DActor;
	// 当前位置寻路索引
	int32 PathfindingIndex = INDEX_NONE;
};
