// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GridTileInterface.h"
#include "ModularPawn.h"
#include "GridActor.generated.h"

class AGridMapManager;

/**
 * AGridActor
 *
 * GridMap 放置类
 * 是其他可以被放置到GridMap中物体的父类
 * 可以获取到他在地图上的位置索引
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTVGAME_API AGridActor : public AModularPawn, public IGridTileInterface
{
	GENERATED_BODY()

public:
	AGridActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ~AActor interface
	virtual void OnConstruction(const FTransform& Transform) override;
	// ~AActor interface

	// ~APawn interface
	virtual void SpawnDefaultController() override;
	// ~APawn interface
	
	// ~GridTileInterface
	virtual int32 MeshToInstance(const AActor* Actor) override;
	virtual FVector GetVectorArray(int32 Index) override;
	virtual int32 VectorToIndex(const FVector& Vector) override;
	// ~GridTileInterface

protected:
	
	// 是否移动到锚点
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid|Options")
	bool bIgnoreSnapToCustomGrid = false;

	// 将 Actor 移动到锚点位置
	UPROPERTY(BlueprintReadWrite, Category="Grid|Options")
	bool bPlaceActorToAnchor = false;

	// 当前所在位置索引
	UPROPERTY(VisibleInstanceOnly, Category="Grid")
	int32 TileIndex = 0;
	
	// 锚点
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Grid|Components")
	TObjectPtr<USceneComponent> CustomGridAnchor;

	UPROPERTY(Transient)
	TObjectPtr<AGridMapManager> GridMapManager;

	UPROPERTY(EditAnywhere, Category="Grid|Options")
	float ZOffset = 0.0f;
};
