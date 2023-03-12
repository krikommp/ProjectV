// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayMessageProcessor.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Templates/UnrealTemplate.h"

void UGameplayMessageProcessor::BeginPlay()
{
	Super::BeginPlay();

	StartListening();
}

void UGameplayMessageProcessor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	StopListening();

	// 移除监听
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	for (FGameplayMessageListenerHandle& Handle : ListenerHandles)
	{
		MessageSubsystem.UnregisterListener(Handle);
	}
	ListenerHandles.Empty();
}

void UGameplayMessageProcessor::StartListening()
{
}

void UGameplayMessageProcessor::StopListening()
{
}

void UGameplayMessageProcessor::AddListenerHandle(FGameplayMessageListenerHandle&& Handle)
{
	ListenerHandles.Add(MoveTemp(Handle));
}

double UGameplayMessageProcessor::GetServerTime() const
{
	if (AGameStateBase* GameState = GetWorld()->GetGameState())
	{
		return GameState->GetServerWorldTimeSeconds();
	}
	else
	{
		return 0.0;
	}

}





