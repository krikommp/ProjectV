// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CursorClickInterface.generated.h"

UINTERFACE()
class UCursorClickInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ICursorClickInterface
 *
 * 接受鼠标点击事件的接口
 */
class PROJECTVGAME_API ICursorClickInterface
{
	GENERATED_BODY()

public:
	virtual void OnCursorClick() = 0;
};
