// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "GameplayMessageProcessor.generated.h"

/**
 * UGameplayMessageProcessor
 *
 * 监听事件组件
 */
UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class PROJECTVGAME_API UGameplayMessageProcessor : public UActorComponent
{
	GENERATED_BODY()

public:
	// ~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~UActorComponent interface

	virtual void StartListening();
	virtual void StopListening();

protected:
	void AddListenerHandle(FGameplayMessageListenerHandle&& Handle);
	double GetServerTime() const;

private:
	TArray<FGameplayMessageListenerHandle> ListenerHandles;
};
