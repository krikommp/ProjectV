// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "Camera/GridCameraAssistInterface.h"
#include "GridPlayerController.generated.h"

/**
 * AGridPlayerController
 *
 * 项目自定义玩家控制器
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class PROJECTVGAME_API AGridPlayerController : public ACommonPlayerController, public IGridCameraAssistInterface
{
	GENERATED_BODY()

public:
	AGridPlayerController();

	
};

UCLASS()
class AGridReplayPlayerController : public AGridPlayerController
{
	GENERATED_BODY()
public:

	virtual void SetPlayer(UPlayer* InPlayer) override;
};
