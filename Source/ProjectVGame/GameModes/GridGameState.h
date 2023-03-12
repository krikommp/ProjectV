// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularGameState.h"
#include "Messages/GridVerbMessage.h"
#include "UObject/Object.h"
#include "GridGameState.generated.h"

class UGridExperienceManagerComponent;
class UGridAbilitySystemComponent;

/**
 * AGridGameState
 *
 * 项目自定义 GameState
 */
UCLASS(Config=Game)
class PROJECTVGAME_API AGridGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGridGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	float GetServerFPS() const { return ServerFPS; }

	// ~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~AActor interface

	// ~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	// ~AGameStateBase interface

	// ~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, Category="Grid|GameState")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }

	// 向所有的客户端发送一条消息
	// 用于发送可丢弃的非重要消息，如客户端加入消息
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="Grid|GameState")
	void MulticastMessageToClients(const FGridVerbMessage Message);

	// 向所有的客户端发送一条消息
	// 用于发送不可丢失的消息
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Grid|GameState")
	void MulticastReliableMessageToClients(const FGridVerbMessage Message);
	
private:
	UPROPERTY()
	TObjectPtr<UGridExperienceManagerComponent> ExperienceManagerComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Grid|GameState")
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

protected:

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UPROPERTY(Replicated)
	float ServerFPS;
};
