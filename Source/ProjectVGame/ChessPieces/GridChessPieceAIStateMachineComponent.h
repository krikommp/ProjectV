// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "UObject/Object.h"
#include "GridChessPieceAIStateMachineComponent.generated.h"

class USMInstance;

/**
 * UGridChessPieceAIStateMachineComponent
 *
 * AI状态机
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTVGAME_API UGridChessPieceAIStateMachineComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UGridChessPieceAIStateMachineComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category="Grid|AIStateMachine")
	static UGridChessPieceAIStateMachineComponent* FindAIStateMachineComponent(const AActor* Actor) { return Actor ? Actor->FindComponentByClass<UGridChessPieceAIStateMachineComponent>() : nullptr; }

	UFUNCTION(BlueprintCallable, Category="Grid|AIStateMachine")
	void StartAIStateMachine();

	UFUNCTION(BlueprintCallable, Category="Grid|AIStateMachine")
	void StopAIStateMachine();
	
	void DoStart() const;
	void DoUpdate(float DeltaTime) const;
	void DoStop() const;
	void DoShutDown();

	
protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void OnChessPieceInitialized();
	
	// AI状态机实例
	UPROPERTY(Transient)
	USMInstance* AIStateMachine;

public:

	FSimpleMulticastDelegate OnAIStateMachineStop;
};
