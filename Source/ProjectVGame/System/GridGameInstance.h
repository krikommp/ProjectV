// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "GridGameInstance.generated.h"

class AGridPlayerController;
class UObject;

/**
 * UGridGameInstance
 *
 * 项目自定义 GameInstance
 */
UCLASS(Config=Game)
class PROJECTVGAME_API UGridGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:

	UGridGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AGridPlayerController* GetPrimaryPlayerController() const;

	virtual bool CanJoinRequestedSession() const override;

protected:

	virtual void Init() override;
	virtual void Shutdown() override;
};
