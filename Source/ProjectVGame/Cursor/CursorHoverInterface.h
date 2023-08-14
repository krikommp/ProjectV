// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CursorHoverInterface.generated.h"

UINTERFACE()
class UCursorHoverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ICursorHoverInterface
 *
 * 接受鼠标悬停事件的接口
 */
class PROJECTVGAME_API ICursorHoverInterface
{
	GENERATED_BODY()

public:
	virtual void OnHoverBegin() = 0;
	virtual void OnHovering() = 0;
	virtual void OnHoverEnd() = 0;
};
