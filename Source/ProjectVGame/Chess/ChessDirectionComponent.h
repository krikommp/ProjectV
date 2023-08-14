// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "ChessDirectionComponent.generated.h"

class AChessDirectionArrow;
struct FTilemapPathFindingBlock;

/**
 * UChessDirectionComponent
 *
 * 描述棋子方向
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UChessDirectionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static UChessDirectionComponent* FindChessDirectionComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UChessDirectionComponent>() : nullptr);
	}

	// 获取所有相邻块的索引
	TArray<int32> GetNeighborIndexes() const;
 	const FTilemapPathFindingBlock& GetNeighborPathfindingBlock(int32 EdgeIndex) const;

	// 设置棋子的朝向
	void SetLookAtIndex(int32 Index);
	FORCEINLINE int32 GetLookAtIndex() const { return LookAtIndex; }

	// 显示所有方向箭头
	void ShowAllDirectionArrows();
	void HideAllDirectionArrows();

	static const FName NAME_ActorFeatureName;

protected:
	// Called when the game starts
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

private:
	// 棋子朝向的方块索引
	int32 LookAtIndex;
	
	// 可视化棋子方向的箭头
	UPROPERTY(Transient)
	TArray<TObjectPtr<AChessDirectionArrow>> DirectionArrows;

	// 箭头模型
	UPROPERTY(EditAnywhere, Category=Chess, meta=(AllowPrivateAccess))
	TObjectPtr<UStaticMesh> ArrowMesh;

	// 箭头材质
	UPROPERTY(EditAnywhere, Category=Chess, meta=(AllowPrivateAccess))
	TObjectPtr<UMaterialInterface> ArrowMat;
};
