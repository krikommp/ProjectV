// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "Character/GridPawnData.h"
#include "Messages/GridVerbMessage.h"
#include "System/GameplayTagStack.h"

#include "GridPlayerState.generated.h"

class AGridPlayerController;
class UGridAbilitySystemComponent;
class UGridPawnData;
class UGridExperienceDefinition;

/** Defines the types of client connected */
UENUM()
enum class EGridPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * AGridPlayerState
 *
 * 项目自定义 PlayerState Class
 */
UCLASS(Config=Game)
class PROJECTVGAME_API AGridPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGridPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerState")
	AGridPlayerController* GetGridPlayerController() const;

	UFUNCTION(BlueprintCallable, Category="Grid|PlayerState")
	UGridAbilitySystemComponent* GetGridAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UGridPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	static const FName NAME_GridAbilityReady;

	void SetPlayerConnectionType(EGridPlayerConnectionType NewType);
	EGridPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	// 向这名玩家发送一条消息
	// 只适用于通知类消息
	UFUNCTION(Client, Unreliable, BlueprintCallable,Category="Grid|PlayerState")
	void ClientBroadcastMessage(const FGridVerbMessage Message);

private:
	void OnExperienceLoaded(const UGridExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_PawnData)
	TObjectPtr<const UGridPawnData> PawnData;

private:
	UPROPERTY(VisibleAnywhere, Category="Grid|PlayerState")
	TObjectPtr<UGridAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	EGridPlayerConnectionType MyPlayerConnectionType;
};
