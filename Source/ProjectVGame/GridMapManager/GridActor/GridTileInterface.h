// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GridTileInterface.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UGridTileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IGridTileInterface
 *
 * 放置到 GridMap 中的 Actor 接口
 */
class PROJECTVGAME_API IGridTileInterface
{
	GENERATED_BODY()

public:
	virtual int32 VectorToIndex(const FVector& Vector) = 0;

	virtual FVector GetVectorArray(int32 Index) = 0;

	virtual int32 MeshToInstance(const AActor* Actor) = 0;
};
