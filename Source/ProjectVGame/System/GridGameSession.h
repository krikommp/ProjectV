// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "UObject/Object.h"
#include "GridGameSession.generated.h"

UCLASS(Config=Game)
class PROJECTVGAME_API AGridGameSession : public AGameSession
{
	GENERATED_BODY()

public:

	AGridGameSession(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
};
