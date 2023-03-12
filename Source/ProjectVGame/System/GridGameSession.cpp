// Fill out your copyright notice in the Description page of Project Settings.


#include "GridGameSession.h"

AGridGameSession::AGridGameSession(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void AGridGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AGridGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}
